# ASOCK

A socket library.


## NOTES

The loop

```
while (loop->num_polls)
{
  us_internal_loop_pre(loop);

  loop->num_ready_polls =
      kevent(loop->fd, NULL, 0, loop->ready_polls, 1024, NULL);

  for (loop->current_ready_poll = 0;
      loop->current_ready_poll < loop_num_ready_polls;
      loop->current_ready_poll++)
  {
    event &= us_poll_events(poll);
    if (events || error)
    {
      us_internal_dispatch_ready_poll(poll, error, events);
    }
  }

  us_interal_loop_post(loop);
}
```

# lldb

`breakpoint set -f <file.ext> -l <line>`
`next`
`step`
`list`
