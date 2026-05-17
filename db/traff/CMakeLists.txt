add_custom_command(
  OUTPUT traff_eventlist.sql
  COMMAND mk_evtlist "TraFF Event List.md")
add_custom_command(
  OUTPUT traff_sillist.sql
  COMMAND mk_evtlist "TraFF SI List.md")

