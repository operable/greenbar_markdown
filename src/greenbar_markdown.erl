-module(greenbar_markdown).

-export([init/0,
         analyze/1,
         parse/1]).

-on_load(init/0).

-define(nif_error, erlang:nif_error(not_loaded)).

init() ->
  case build_nif_path() of
    {ok, Path} ->
      erlang:load_nif(Path, undefined);
    Error ->
      Error
  end.

analyze(Text) when is_list(Text) ->
  analyze(iolist_to_binary(Text));
analyze(Text) when is_binary(Text) ->
  case parse(Text) of
    {ok, []} ->
      {ok, []};
    {ok, Values} ->
      {ok, lists:reverse(Values)};
    Error ->
      Error
  end.

parse(_Text) -> ?nif_error.

build_nif_path() ->
  case escript_path() of
    undefined ->
      case code:priv_dir(greenbar_markdown) of
        Path when is_list(Path) ->
          {ok, filename:join([Path, "greenbar_markdown"])};
        {error, bad_name} ->
          case code:which(?MODULE) of
            Filename when is_list(Filename) ->
              {ok, filename:join([filename:dirname(Filename),
                                  "..","priv",
                                  "greenbar_markdown"])};
            Reason when is_atom(Reason) ->
              {error, Reason}
          end
      end;
    EscriptPath ->
      SharedLibPath = filename:join([EscriptPath, "greenbar_markdown"]),
      {ok, SharedLibPath}
  end.

escript_path() ->
  case catch escript:script_name() of
    {'EXIT', _} ->
      undefined;
    %% Running inside iex
    "--no-halt" ->
      undefined;
    Path ->
      case filename:basename(Path) of
        "mix" ->
          %% Running under mix
          undefined;
        _ ->
          filename:dirname(filename:absname(Path))
      end
  end.
