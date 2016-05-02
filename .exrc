set expandtab
set sw=4
let g:ycm_confirm_extra_conf=0
set errorformat^=%-G%.%#Built%.%#
set errorformat^=%-G%.%#Building%.%#
map <Leader>t :Dispatch cd BUILD; ctest -j32<cr>
map <Leader>u :Dispatch BUILD/tests/unittest<cr>
set makeprg=make\ -C\ BUILD\ --no-print-directory\ -j6;\ BUILD/tests/unittest
map <Leader>m :Make<cr>
map <Leader>M :Make!<cr>
map <Leader>c :Copen<cr>
