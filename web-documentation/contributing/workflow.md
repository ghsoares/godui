# Workflow

## Github

Feel free to contribute to Godui, it's free and open-source, you can view the source code, open issues, pull requests and more at the [Github repository](https://github.com/ghsoares/godui).

### Pull request

Before making any changes to Godui's source code, you must fork the Godui's repository into your Github's profile, clone it to your machine and make changes from there.

After making changes to Godui's source code, tested and avaliated as ready to use, you can submit a pull request at [Github repository's pull request](https://github.com/ghsoares/godui/issues) which will be evaluated and merged if possible.

### Issues

You can open issues at [Github repository's issues](https://github.com/ghsoares/godui/issues) for the problems or bugs you've encountered when using Godui.

## GDExtension

Godui is made using Godot's GDExtension, which provides a more direct access to the game engine's API from C++ without recompiling the entire engine. All the source code for the extension can be found at `gdextension/src/` folder.

Instructions about compiling the GDExtension can be found at "[compiling](contributing/compiling.md)" page.

## Documentation

Godui's online documentation uses [Docsify](https://docsify.js.org/) to easily generate html at runtime using markdown, all the relevant files can be found at `web-documentation` folder inside the repository.

### Class reference

For the Godui's custom classes references, I've made a script that parses the markdown to extract the class's methods and properties information, it can be found at `web-documentationjs/apify.js`, it consists of some HTML tags to describe the class:

- `<span class-ref>{class}</span>`: Inserts a link to the Godot's class reference based on `{class}` name, the only exceptions are `void` and Godui's classes (must be inserted at `apify.js:CLASS_REFERENCE` object);

- `<param parse-class-api/>`: Marks a markdown file to parse other class reference tags;

- `<h3 class-property>{type} {name}</h3>`: Creates the markdown for a property and will be added to `class-methods` tag, optionally you can pass `readonly` inside the tag (`<h3 class-property readonly>`) to only generate the getter of property, and assign a default value to it (`{type} {name} = {value}`);

- `<h3 class-method>{type} {name}({param_type} {param_name})</h3>`: Creates the markdown for a method and will be added to `class-properties` tag, optionally you can pass default arguments (`{param_type} {param_name} = {value}`) and flags (`{type} {name}({param_type} {param_name}) {flag} </h3>` where `{flag}` can be `const` or `static`);

- `<span class-properties>{msg}</span>`: After extracting all the class's properties, it inserts a table containing all the properties and links to then in the same page. `{msg}` is used to display a fallback message when not viewing from Docsify;

- `<span class-methods>{msg}</span>`: After extracting all the class's methods, it inserts a table containing all the methods and links to then in the same page. `{msg}` is used to display a fallback message when not viewing from Docsify;

- `<span class-method-ref>{method}</span>`: Links to the description of a method of this class, or another  class if you provide it's name (`{class_name}:{method}`);

- `<span class-property-ref>{property}</span>`: Links to the description of a property of this class, or another class if you provide it's name (`{class_name}:{property}`).

I suggest to insert class references inside `web-documentationapi/` folder and update `web-documentation_sidebar.md` to include the newly added class reference (inside `API` section).