The `Array` object stores an ordered sequence of values in a dense array. It has a subset of the Ruby array methods, currently including none of the mutating methods.

Scripts can create array literals using square brackets, e.g. `[]` (empty array), `[5]`, `[5, true, "string"]`.

Most methods are the same as the Ruby array methods.

   * `assoc obj`
   * `at`
   * `compact`
   * `count`, `count obj`: count with a block is not supported.
   * `each`
   * `empty?`
   * `fetch index`, `fetch index, default`: fetch with a block is not supported.
   * `first`, `first n`
   * `flatten`, `flatten level`
   * `frozen?`: always returns true.
   * `include? obj`
   * `index obj`, `find_index obj`: Block or enumerator is not supported.
   * `join sep`: sep must be specified.
   * `last`, `last n`
   * `length`, `size`
   * `rassoc obj`
   * `reverse`
   * `rindex obj`: Block or enumerator is not supported.
   * `rotate count=1`
   * `slice index`, `slice start, length`: Range is not supported.
   * `sort`: Sort using each elements `<=>`: Block is not supported.
   * `take n`
   * `uniq`: Unique elements using `==`: Block is not supported.
   * `values_at indices...`: Range is not supported.

## Methods

### `inspect`
An array literal like rendering using square brackets, and comma separated list of values, each converted to a string using `inspect`.

    [5, true, "string"].inspect == "[5, true, \"string\"]"

### `array == array`, `!=`
Two arrays are equal if both are the same length, and all elements in order are also equal.

    [1, "a", 5] == [1, "a", 5] #true
    [1, "a", "5"] == [1, "a", 5] #false
    [1, 2] == [1, 2, 3] #false
    [1, 2] == [2, 1] #false

### `array <=> array`
Comparison of two arrays if done by an element wise comparison of the arrays. This means that all elements in both arrays must also be comparable.

    [1, 2, 3] <=> [1, 2, 3] #0
    [1, 2] <=> [1, 2, 3] #-1
    [1, 2, 3] <=> [1, 2] #1
    [1, 2, 3] <=> [1, 3, 4] #-1

