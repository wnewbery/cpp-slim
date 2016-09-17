# C++ Slim

This is a C++ implementation of a subset of the Ruby [Slim](http://slim-lang.com/) templating
engine syntax, and Ruby programming language scripts for dynamic content.

Slim:

    html
      head
        title #{@page_title} - C++ Slim
        =yield :head
      body
        header
          h1 = @page_title
        = yield
        footer
          | Copyright &copy; #{year} #{@authors.join ', '}

C++:

    //Parse the template files. Templates can be re-used and are thread-safe.
    auto page_tpl = slim::parse_template_file("example.html.slim")
    auto layout_tpl = slim::parse_template_file("layout.html.slim")
    
    //ViewModel contains all the methods and instance data for "self" in the template/scripts.
    auto model = std::make_shared<MyViewModel>();
    model->set("page_title", slim::make_value("Example"));
    
    //Render to a HTML string
    auto html = page_tpl->render_layout(layout_tpl, model);

# [Template Syntax](Template.md)
The template syntax is based on the Ruby [Slim](http://slim-lang.com/) templating engine.

# [Script/Expression Syntax](Script.md)
The templates can include simple Ruby-like script expression for the creation of dynamic content.

This is supported by a C++ object hierarchy (starting with `slim::Object`) and a Ruby based source text syntax.
