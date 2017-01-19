// -*- coding:utf-8;Mode:C;tab-width:2;c-basic-offset:2;indent-tabs-mode:nil -*-
// -------------------------------------------------------------------
//
// Copyright (c) 2016 Operable, Inc. All Rights Reserved
//
// This file is provided to you under the Apache License,
// Version 2.0 (the "License"); you may not use this file
// except in compliance with the License.  You may obtain
// a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.
//
// ------------------------------------------------------------------
#include <assert.h>
#include <iostream>
#include <string>

#include "erl_nif.h"
#include "buffer.h"
#include "gb_common.hpp"
#include "markdown_analyzer.hpp"

// Prototype
#define NIF(name) \
  ERL_NIF_TERM name(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])

// Preferred write size for hoedown's output buffer
#define OUTPUT_SIZE 128

// NIF function forward declares
NIF(gb_parse);


static ErlNifFunc nif_funcs[] =
{
  {"parse", 1, gb_parse, 0}
};

static ERL_NIF_TERM make_atom(ErlNifEnv* env, const char* name) {
  ERL_NIF_TERM atom;
  if (enif_make_existing_atom(env, name, &atom, ERL_NIF_LATIN1) == false) {
    atom = enif_make_atom(env, name);
  }
  return atom;
}

static int on_load(ErlNifEnv* env, void** priv, ERL_NIF_TERM load_info) {
  gb_priv_s* priv_data = (gb_priv_s*) enif_alloc(sizeof(gb_priv_s));

  // Allocating private data failed, so abort loading the NIF
  if (priv_data == NULL) {
    return 1;
  }

  // Frequently used atoms
  priv_data->gb_atom_ok = make_atom(env, "ok");
  priv_data->gb_atom_error = make_atom(env, "error");
  priv_data->gb_atom_out_of_memory = make_atom(env, "out of memory");

  *priv = (void *) priv_data;
  return 0;
}

static int on_upgrade(ErlNifEnv* env, void** priv_data, void** old_priv_data, ERL_NIF_TERM load_info) {
  return on_load(env, priv_data, load_info);
}

static void on_unload(ErlNifEnv* env, void* priv) {
  enif_free(priv);
}

static ERL_NIF_TERM convert_results(ErlNifEnv *env, greenbar::node2::NodeVector *collector) {
  ERL_NIF_TERM head, tail;
  tail = enif_make_list(env, 0);
  if (collector->size() < 1) {
    return tail;
  }
//  assert(collector->size() == 1);
  size_t last_index = collector->size() - 1;
  for(size_t i = 0; i < collector->size(); i++) {
    auto info = collector->at(i);

    // Don't add double EOLs to end of template
    if (i == last_index && info->get_type() == greenbar::node2::MD_EOL) {
      auto previous = collector->at(i - 1);
      if (previous->get_type() == greenbar::node2::MD_EOL) {
        continue;
      }
    }
    head = info->to_erl_term(env);
    tail = enif_make_list_cell(env, head, tail);
  }

  return tail;
}

NIF(gb_parse) {
  gb_priv_s *priv_data = (gb_priv_s*) enif_priv_data(env);
  ErlNifBinary input;
  if (enif_inspect_binary(env, argv[0], &input) == 0) {
    return enif_make_badarg(env);
  }

  hoedown_buffer* ob = hoedown_buffer_new(OUTPUT_SIZE);
  if (ob == NULL) {
    return priv_data->gb_atom_out_of_memory;
  }
  auto analyzer = greenbar::new_markdown_analyzer();
  if (analyzer == NULL) {
    hoedown_buffer_free(ob);
    return priv_data->gb_atom_out_of_memory;
  }
  hoedown_document* document = greenbar::new_hoedown_document(analyzer);
  if (document == NULL) {
    greenbar::free_markdown_analyzer(analyzer);
    hoedown_buffer_free(ob);
    return priv_data->gb_atom_out_of_memory;
  }
  hoedown_document_render(document, ob, (uint8_t*) input.data, input.size);

  enif_release_binary(&input);
  auto collector = greenbar::get_collector(analyzer);
  auto result = convert_results(env, collector);
  greenbar::free_markdown_analyzer(analyzer);
  hoedown_document_free(document);
  return enif_make_tuple(env, 2, priv_data->gb_atom_ok, result);

}

ERL_NIF_INIT(greenbar_markdown, nif_funcs, on_load, NULL, on_upgrade, on_unload)
