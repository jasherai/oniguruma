require 'mkmf'
have_library("onig")
$CFLAGS='-Wall'
create_makefile( "oregexp" )
