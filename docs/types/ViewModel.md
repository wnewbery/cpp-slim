#ViewModel

The `ViewModel` class is the base type for the `self` object for all template and script executions, as it implements the required attribute and constant table functionality.

It has a similar purpose to the instance used by Rails when rendering views, providing the template access to attributes (via @, e.g. `@page_title`) provided by the controller, helper methods and constants/imported modules (e.g. `Math`).


## Methods
### `content_for symbol ||`
### `yield`, `yield symbol`
