pkg = import('pkgconfig')

sources = [
  'ebuf.c',
  'hash.c',
  'collections/array.c',
  'collections/list.c',
  'collections/interop.c',
  'collections/hashmap.c',
  'math/prime.c',
]

magpie = library(
  'magpie',
  include_directories: inc,
  sources: sources,
  version: '0.0.1',
  soversion: '0',
  install: true
)

pkg.generate(magpie)

headers = [
  'ebuf.h',
  'collections/array.h',
  'collections/list.h',
  'collections/interop.h'
]

install_headers(headers, subdir: 'magpie', preserve_path: true)

