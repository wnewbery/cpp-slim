#Slim Template
The main templating system provide by this project (and indeed the reason it exists) is based on <http://slim-lang.com/>, and tries to maintain compatibility with the subset it implements.
There is no intention to implement all of Ruby, use an actual Ruby implementation with the Slim gem for that.

Currently the template always renders as if for HTML5.

The following parts of the Slim syntax is implemented. The Slim documentation page (<http://www.rubydoc.info/gems/slim/frames>) contains full detail about them.

# Line indicators
## Verbatim text |
Verbatim text with the pipe, single quote or inline HTML is supported.
Interpolation within these blocks using the Ruby syntax (e.g. `#{@my_expr + 5}`) is also supported.
Any expression result that is not an instance of `HtmlSafeType` (e.g. String `String#html_safe`) will be HTML encoded.


## Control code -
The control codes `- if <expr>`, `- elsif <expr>`, `- else <expr>` are supported. Loops and arbitary code are not yet supported.

## Output =
The output line is supported and may contain a single script expression. As with string interpolation, the overall result of the expression will be escaped unless it is a `HtmlSafeString`.

`=>`, `=<`, `=<>` are supported to control whitespace.

## Output without HTML escaping == (Not supported)
`==` lines are not supported. Use `=` lines but return a `HtmlSafeString` using `String#html_safe` instead.

# Code comment /
# HTML comment /!
Comments are supported.

# IE conditional comment /[...] (Not supported)
IE conditional comments are not supported. If they are really needed, use the inlne HTML syntax.

    <!--[if IE]>conditional<![endif]-->

# HTML tags
## <!DOCTYPE> declaration (Not supported)
Note that the HTML5 doctype can be requested when calling `Template::render`.

## Closed tags (trailing /) (Not supported)
Note that the standard void tags such as `img` and `br` are implicitly closed.

## Trailing and leading whitespace (<, >)
Whitespace control for tags is the same as for output code, `<`, `>` and `<>`.

## Inline tags (Not supported)

## Text content
Text content for tags on the same line works as with verbatim text, and will escape interpolation unless a `HtmlSafeString`.

The "smart text" mode is not supported.

    body
      h1 My Site


##Dynamic content (= and ==)
Dynamic content with `=` is supported mostly the same as `Output =` but without block support.
This includes `<`, `>` and `<>` whitespace control.

As with output lines, `==` is not supported, use `HtmlSafeString`.

##Attributes
### Attributes wrapper (Not supported)
### Quoted attributes
Quoted attributes are parsed as if Ruby/script attributes (a string literal).
This means that interpolation is supported, but as a single Ruby expression, the entire value will be escaped unless a `HtmlSafeString`, not just interpolated blocks.

### Ruby attributes
Attribute values are parsed as a single script expression, and so may be string literal with interpolation, method calls, attributes, etc.

    input type="text" values=@current_user&.name

### Boolean attributes
If the attribute value expression results in `true`, then the attribute is added as a boolean attribute with just its name.
If the attribute value is `false` or `nil` then the attribute is considered a boolean attribute and skipped entirely.

    input disabled=true
    input disabled=false
    input disabled="not a bool"
    input disabled=returns_true()
    
    <input disabled />
    <input />
    <input disabled="not a bool" />
    <input disabled />

### Attribute merging
If there is multiple attributes with the same name, or if the attribute values executes to an `Array` instance.
Then all the values will be joined in a space seperated list, and a single attribute will be written as output.
An empty array will result in an attribute with an empty-string value.

    div.a.b class="c d"
    <div class="a b c d"></div>

The Slim `:merge_attrs` configuration is not supported.

###Splat attributes * (Not supported)
###Dynamic tags * (Not supported)
# Shortcuts
## Tag Shortcuts (Not supported)
## Attribute shortcuts
The `#` and `.` shortcuts for the `id` and `class` attributes are supported, but a tag name must be specified.
There is not support for specifiying custom shortcuts.

#Helpers, capturing and includes
Additional helper methods and modules can be added by subclassing the `ViewModel` class used to render the view.

The `ViewModel` class itself contains a capturing `content_for` method and `yield` for use with layouts (along with `Template::render_layout`).

#Text interpolation
Interpolation is supported in attribute strings and verbatim text, as described by those sections.
Dynamic content is always escaped unless an instance of `HtmlSafeString`.

#Embedded engines (Markdown, ...) (Not supported)
