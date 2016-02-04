#!/bin/sh

# Get memory usage every X seconds
INTERVAL=1
# Either record an existing PID or whatever is given after options
PID=
# A human readable string for what's being recorded
TARGET=
# The file in which RSS numbers are written
LOG=

IS_OSX=0
if [ $(uname) = "Darwin" ]; then
  IS_OSX=1
fi

get_progam() {
  local pid=$1
  echo $(ps -p "$pid" -o command=)
}

is_alive() {
  local pid=$1
  ps -p $pid > /dev/null 2>&1
}

get_rss_kb() {
  local pid=$1
  if [ $IS_OSX -eq 1 ]; then
    ps -p $pid -o rss=
  else
    echo $(($(cat /proc/$pid/statm | cut -d" " -f2) * 4))
  fi
}

while getopts "i:l:p:" option; do
  case $option in
    i)
      INTERVAL=$OPTARG
    ;;
    l)
      LOG=$OPTARG
    ;;
    p)
      PID=$OPTARG
    ;;
    ?)
      usage "Unrecognized option: $option"
    ;;
  esac
done

shift $(($OPTIND - 1))

if [ x"$LOG" = "x" ]; then
  LOG=$(mktemp /tmp/memprof.XXXXXX)
fi

if [ x"$PID" = "x" ]; then
  TARGET="$@"
else
  TARGET="PID $PID ($(get_progam $PID))"
fi

echo "Recording '$TARGET' into $LOG" 1>&2
echo "# RSS in kB every $INTERVAL seconds of '$TARGET'" > $LOG

if [ x"$PID" = "x" ]; then
  $@ &
  PID=$!
fi

while is_alive $PID; do
  rss=$(get_rss_kb $PID)
  echo $rss >> $LOG
  sleep $INTERVAL
done
