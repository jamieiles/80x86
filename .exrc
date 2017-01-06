set expandtab
set sw=4
let g:ycm_confirm_extra_conf=0
set errorformat^=%-G%.%#Built%.%#
set errorformat^=%-G%.%#Building%.%#
map <Leader>t :Dispatch ./docker/s80x86-dev "cd _build/dev; ninja && ninja test"<cr>
map <Leader>u :Dispatch ./docker/s80x86-dev "cd _build/dev; ninja && ./tests/unittest"<cr>
set makeprg=./docker/s80x86-dev\ \"cd\ _build/dev;\ ninja\"
map <Leader>m :Make<cr>
map <Leader>M :Make!<cr>
map <Leader>c :Copen<cr>
