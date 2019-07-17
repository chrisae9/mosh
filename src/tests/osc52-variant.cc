/*
    Mosh: the mobile shell
    Copyright 2026 Chris Alves

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
*/

#include <cstdlib>
#include <iostream>
#include <string>

#include "src/terminal/parser.h"
#include "src/terminal/terminal.h"

static void feed_sequence( Terminal::Emulator& terminal, const std::string& sequence )
{
  Parser::UTF8Parser parser;

  for ( char ch : sequence ) {
    Parser::Actions actions;
    parser.input( ch, actions );
    for ( const auto& action : actions ) {
      action->act_on_terminal( &terminal );
    }
  }
}

static Terminal::Framebuffer::title_type title_from_string( const std::wstring& value )
{
  return Terminal::Framebuffer::title_type( value.begin(), value.end() );
}

static void expect_clipboard( const std::string& sequence, const Terminal::Framebuffer::title_type& expected )
{
  Terminal::Emulator terminal( 80, 24 );
  feed_sequence( terminal, sequence );

  const auto& clipboard = terminal.get_fb().get_clipboard();
  if ( clipboard != expected ) {
    std::cerr << "clipboard mismatch: expected length " << expected.size() << " got length " << clipboard.size() << "\n";
    std::exit( 1 );
  }
}

int main()
{
  const auto payload = title_from_string( L"SGVsbG8=" );

  expect_clipboard( "\033]52;c;SGVsbG8=\007", payload );
  expect_clipboard( "\033]52;c:tmux;SGVsbG8=\007", payload );
  return 0;
}
