const TYPE_REFERENCE = {
	"float": "https://docs.godotengine.org/en/stable/classes/class_float.html",
	"int": "https://docs.godotengine.org/en/stable/classes/class_int.html",
	"bool": "https://docs.godotengine.org/en/stable/classes/class_bool.html",
	"String": "https://docs.godotengine.org/en/stable/classes/class_string.html",
	"Dictionary": "https://docs.godotengine.org/en/stable/classes/class_dictionary.html",
	"Array": "https://docs.godotengine.org/en/stable/classes/class_array.html",
	"Callable": "https://docs.godotengine.org/en/stable/classes/class_callable.html",
	"Variant": "https://docs.godotengine.org/en/stable/classes/class_variant.html",
	"Object": "https://docs.godotengine.org/en/stable/classes/class_object.html",
	"RefCounted": "https://docs.godotengine.org/en/stable/classes/class_refcounted.html",
	"Node": "https://docs.godotengine.org/en/stable/classes/class_node.html",
	"Control": "https://docs.godotengine.org/en/stable/classes/class_control.html",
	"Container": "https://docs.godotengine.org/en/stable/classes/class_container.html",
	"ScrollContainer": "https://docs.godotengine.org/en/stable/classes/class_scrollcontainer.html",
	"Button": "https://docs.godotengine.org/en/stable/classes/class_button.html",
	"LineEdit": "https://docs.godotengine.org/en/stable/classes/class_lineedit.html",
	"UI": "api/ui.md",
	"MotionRef": "api/motion_ref.md"
};

const markdownType = (type) => {
	if (TYPE_REFERENCE[type]) {
		return `[${type}]`;
	} else {
		return type;
	}
}

(function () {
	var apifyPlugin = function (hook, vm) {
		hook.beforeEach(function (markdown, next) {
			let parseAPI = false;
			
			markdown = markdown.replace(/\<\w+ parse-class-api\/>/, () => {
				parseAPI = true;
				return "";
			});

			markdown = markdown.replace(/\<\w+ class-reference\/>/g, () => {
				return Object.entries(TYPE_REFERENCE).map(([name, url]) => `[${name}]: ${url}`).join("\n");
			});

			if (parseAPI) {
				let methods = [];
	
				markdown = markdown.replace(/<\w+ class-method>(\w+) +(\w+) *\(((?:[\w ,.=]|\\")*)\) *([\w ]+)?<\/\w+>/g, (s, ret, name, args, flags) => {
					console.log(ret, name, args, flags);

					s = `### ${name}\n`;
	
					s += `**Signature**: ${markdownType(ret)} ${name} (`;
					let methodTableSignature = `[${name}](#${name}) (`
					let firstArg = true;
					for (const arg of args.matchAll(/(\w+) (\w+)(?: *= *([a-zA-Z0-9.]+))?/g)) {
						if (!firstArg) {
							s += ", ";
							methodTableSignature += ", ";
						}
						s += `${markdownType(arg[1])} ${arg[2]}`;
						methodTableSignature += `${markdownType(arg[1])} ${arg[2]}`;
						if (arg[3]) {
							s += ` = ${arg[3]}`;
							methodTableSignature += ` = ${arg[3]}`;
						}
						firstArg = false;
					}
					s += ")";
					methodTableSignature += ")";
	
					if (flags) {
						s += " " + flags;
						methodTableSignature += " " + flags;
					}
	
					methods.push({
						ret: markdownType(ret),
						signature: methodTableSignature
					});
					return s;
				});
	
				let methodsTable = methods.map(m => {
					return `| ${m.ret} | ${m.signature} |`
				});
	
				markdown = markdown.replace(/<\w+ class-methods>.*<\/\w+>/, () => `| Returns | Method |\n| --- | --- |\n${methodsTable.join("\n")}`);
			}

			next(markdown);
		});
	};

	// Add plugin to docsify's plugin array
	$docsify = $docsify || {};
	$docsify.plugins = [].concat(apifyPlugin, $docsify.plugins || []);
})();