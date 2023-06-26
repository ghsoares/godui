# Workflow

Feel free to contribute to Godui, it's free and open-source, you can view the source code, open issues, pull requests and more at the [Github repository](https://github.com/ghsoares/godui).

## Documentation

Godui's online documentation uses [Docsify](https://docsify.js.org/) to easily generate html at runtime using markdown, all the relevant files can be found at `docs/` folder inside the repository.

### Class reference

For the Godui's custom classes references, I've made a script that parses the markdown to extract the class's methods and properties information, it can be found at `docs/js/apify.js`, it consists of some HTML tags to describe the class:

- `<span class-ref>{class}</span>`: Inserts a link to the Godot's class reference based on `{class}` name, the only exceptions are `void` and Godui's classes (must be inserted at `apify.js:CLASS_REFERENCE` object);
- `<span parse-class-api/>`: Marks a markdown file to parse other class reference tags;
- `<h3 class-property>{type} {name}</h3>`: Creates the markdown for a property and will be added to `class-methods` tag, optionally you can pass `readonly` inside the tag (`<h3 class-property readonly>`) to only generate the getter of property, and assign a default value to it (`{type} {name} = {value}`);
- `<h3 class-method>{type} {name}({param_type} {param_name})</h3>`: Creates the markdown for a method and will be added to `class-properties` tag, optionally you can pass default arguments (`{param_type} {param_name} = {value}`) and flags (`) {flag} </h3>` where `{flag}` can be `const` or `static`);
- `<span class-properties>{msg}</span>`: After extracting all the class's properties, it inserts a table containing all the properties and links to then in the same page. `{msg}` is used to display a fallback message when not viewing from Docsify;
- `<span class-methods>{msg}</span>`: After extracting all the class's methods, it inserts a table containing all the methods and links to then in the same page. `{msg}` is used to display a fallback message when not viewing from Docsify.