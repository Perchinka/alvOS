[bits 32]
[extern kernel_main]

global kernel_entry

kernel_entry:
  call kernel_main
  hlt
