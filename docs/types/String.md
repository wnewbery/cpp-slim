# Type: String

The `String` class represents UTF-8 strings and is used for String literals. Most of the functionality is similar to Ruby strings, but other encodings and mutator methods are not implemented.

Methods regarding indicies or lengths always work with UTF-8 elements, not "characters" or codepoints, unlike Ruby.

Most methods are the same as the Ruby methods.

   * `to_f`, `to_d`: As a number converted using `std::stod`.
   * `to_i`: As a number converted using `std::stoi`.
   * `to_sym`: Returns a `Symbol` with the same value.
   * `[]`: Range is not supported.
   * `ascii_only?`: True if all UTF-8 elements are in the range 0 to 127.
   * `bytes`
   * `byteslice`
   * `capitalize`
   * `casecmp string`
   * `center width, pad_str=" "`
   * `ljust width, pad_str=" "`
   * `rjust width, pad_str=" "`
   * `chars`
   * `chop`
   * `chomp`, `chomp separator`: Acts as if `$/` is the default
   * `chr`
   * `codepoints`
   * `downcase`
   * `each_byte {|int| block} → str`, `each_byte → Enumerator`
   * `each_char {|str| block} → str`, `each_char → Enumerator`
   * `each_codepoint {|int| block} → str`, `each_codepoint → Enumerator`
   * `each_line {|line| block} → str`, `each_line sep {|line| block} → str`,
     `each_line → Enumerator`, `each_line sep → Enumerator`.
     The default separator is always `\n`.
   * `empty?`
   * `end_with? ([suffixes+])`
   * `getbyte index`
   * `hex`
   * `include? string`
   * `index`
   * `lines separator="\n"`: The default separator is always `\n`.
   * `lstrip`
   * `ord`
   * `partition string`
   * `upcase`
   * `reverse`
   * `rpartition string`
   * `rindex`
   * `scrub replacement=U+FFFD`
   * `size`, `length`
   * `split`: The default pattern is always " ".
   * `start_with? ([prefixes+])`
   * `strip`
   * `rstrip`

## Methods
### `html_safe`
Creates a `HtmlSafeString` containing this string.
