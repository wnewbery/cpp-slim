# Type: String

The `String` class represents UTF-8 strings and is used for String literals. Most of the functionality is similar to Ruby strings, but other encodings and mutator methods are not implemented.

Most methods are the same as the Ruby methods.

   * `to_f`, `to_d`: As a number converted using `std::stod`.
   * `to_i`: As a number converted using `std::stoi`.
   * `to_sym`: Returns a `Symbol` with the same value.
   * `[index]`, `[match_str]`, `[start, length]`: Regex and range is not supported.
   * `ascii_only?`: True if all UTF-8 elements are in the range 0 to 127.
   * `capitalize`
   * `casecmp string`
   * `center width, pad_str=" "`
   * `ljust width, pad_str=" "`
   * `rjust width, pad_str=" "`
   * `chomp`, `chomp separator`: Acts as if `$/` is the default
   * `downcase`
   * `each_line |line|`, `each_line sep |line|`. The default separator is always `\n`.
   * `empty?`
   * `end_with? ([suffixes+])`
   * `hex`
   * `include? string`
   * `index substring, offset=0`: Regex is not supported.
   * `lines separator="\n"`: The default separator is always `\n`.
   * `lstrip`
   * `ord`
   * `partition string`
   * `upcase`
   * `reverse`
   * `rpartition string`
   * `rindex substring`, `rindex substring, offset`: Regex is not supported.
   * `size`, `length`
   * `split pattern=" ", limit=0`: Regex is not supported. The default pattern is always " ".
   * `start_with? ([prefixes+])`
   * `strip`
   * `rstrip`

## Methods
### `html_safe`
Creates a `HtmlSafeString` containing this string.
