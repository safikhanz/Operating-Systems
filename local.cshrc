# @(#)cshrc 1.11 89/11/29 SMI
umask 022
if ( $?prompt ) then
   set history=32
endif

set path=($path . ~/bin)

