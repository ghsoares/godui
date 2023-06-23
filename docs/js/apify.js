(function () {
	var apifyPlugin = function (hook, vm) {
		// Invoked on each page load before new markdown is transformed to HTML.
		// Supports asynchronous tasks (see beforeEach documentation for details).
		hook.beforeEach(function (markdown, next) {
			markdown = markdown.replace(/\[class_reference\]/g, () => {
				return [
					"[float]: https://docs.godotengine.org/en/stable/classes/class_float.html",
					"[int]: https://docs.godotengine.org/en/stable/classes/class_int.html",
					"[bool]: https://docs.godotengine.org/en/stable/classes/class_bool.html",
					"[String]: https://docs.godotengine.org/en/stable/classes/class_string.html",
					"[Dictionary]: https://docs.godotengine.org/en/stable/classes/class_dictionary.html",
					"[Callable]: https://docs.godotengine.org/en/stable/classes/class_callable.html",
					"[Variant]: https://docs.godotengine.org/en/stable/classes/class_variant.html",
					"[Object]: https://docs.godotengine.org/en/stable/classes/class_object.html",
					"[RefCounted]: https://docs.godotengine.org/en/stable/classes/class_refcounted.html",
					"[Node]: https://docs.godotengine.org/en/stable/classes/class_node.html",
					"[Control]: https://docs.godotengine.org/en/stable/classes/class_control.html",
					"[UI]: api/ui.md",
				].join("\n");
			});
			next(markdown);
		});
	};

	// Add plugin to docsify's plugin array
	$docsify = $docsify || {};
	$docsify.plugins = [].concat(apifyPlugin, $docsify.plugins || []);
})();