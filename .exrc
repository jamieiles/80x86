set expandtab
set sw=4
let g:ycm_confirm_extra_conf=0
set errorformat^=%-G%.%#Built%.%#
set errorformat^=%-G%.%#Building%.%#
set errorformat^=%-G%.%#sec%.%#
let g:dispatch_compilers = {
    \ './docker/s80x86-dev': ''}
set makeprg=./docker/s80x86-dev
map <Leader>t :Make "ninja -C _build/dev && ninja -C _build/dev test"<cr>
map <Leader>u :Make "ninja -C _build/dev && ./_build/dev/tests/unittest"<cr>
map <Leader>m :Make<cr>
map <Leader>M :Make!<cr>
map <Leader>c :Copen<cr>
au BufNewFile,BufRead *.us set filetype=asm
