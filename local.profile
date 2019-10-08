# @(#)local.profile 1.8 99/03/26 SMI
#
#
stty istrip

#
# If possible, start the windows system
#
if [ "`tty`" = "/dev/console" ] ; then
   if [ "$TERM" = "sun" -o "$TERM" = "sun-color" -o "$TERM" = "AT386" ] ; then
      if [ ${OPENWINHOME:-""} = "" ] ; then
         OPENWINHOME=/usr/openwin
         export OPENWINHOME
      fi

      echo ""
      echo "Starting OpenWindows in 5 seconds (type Control-C to interrupt)"
      sleep 5
      echo ""
      $OPENWINHOME/bin/openwin
      clear    # get rid of annoying cursor rectangle
      exit     # logout after leaving windows system

   fi
fi

PATH=/usr/bin:/usr/ucb:/etc:.:$HOME/bin
export PATH
