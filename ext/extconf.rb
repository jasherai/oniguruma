require 'mkmf'
dir_config("onig")
if have_library("onig")
  $CFLAGS='-Wall'
  create_makefile( "oregexp" )
else
  puts "Cannot find lib-oniguruma. Set location with e.g. --with-onig-dir=/opt/local"
  exit 1
end