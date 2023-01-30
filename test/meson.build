cunit = dependency('cunit')

arrays = executable(
  'magpie_array',
  sources: 'test_arrays.c',
  include_directories: inc,
  link_with: magpie,
  dependencies: cunit
)

linked_lists = executable(
  'magpie_lists',
  sources: 'test_lists.c',
  include_directories: inc,
  link_with: magpie,
  dependencies: cunit
)

test('test arrays', arrays)
test('test linked lists', linked_lists)
