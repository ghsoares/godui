const TYPE_REFERENCE = {
	"float": "https://docs.godotengine.org/en/stable/classes/class_float.html",
	"int": "https://docs.godotengine.org/en/stable/classes/class_int.html",
	"bool": "https://docs.godotengine.org/en/stable/classes/class_bool.html",
	"String": "https://docs.godotengine.org/en/stable/classes/class_string.html",
	"Dictionary": "https://docs.godotengine.org/en/stable/classes/class_dictionary.html",
	"Callable": "https://docs.godotengine.org/en/stable/classes/class_callable.html",
	"Variant": "https://docs.godotengine.org/en/stable/classes/class_variant.html",
	"Object": "https://docs.godotengine.org/en/stable/classes/class_object.html",
	"RefCounted": "https://docs.godotengine.org/en/stable/classes/class_refcounted.html",
	"Node": "https://docs.godotengine.org/en/stable/classes/class_node.html",
	"Control": "https://docs.godotengine.org/en/stable/classes/class_control.html",
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
			markdown = markdown.replace(/\[ParseClassAPI\]/, () => {
				parseAPI = true;
				return "";
			});

			if (parseAPI) {
				markdown = markdown.replace(/\[ClassReference\]/g, () => {
					return Object.entries(TYPE_REFERENCE).map(([name, url]) => `[${name}]: ${url}`).join("\n");
				});
	
				let methods = [];
	
				markdown = markdown.replace(/\[Method\]\((\w+) (\w+)\(((?:\w+ \w+ *=? *(?:[a-zA-Z0-9.]+)?,? *)*)\) *(.+)?\)/g, (s, ret, name, args, flags) => {
					s = `### ${name}\n`;
	
					let methodSignature = `${markdownType(ret)} ${name} (`;
					let methodTableSignature = `${markdownType(ret)} [${name}](#${name}) (`
					let firstArg = true;
					for (const arg of args.matchAll(/(\w+) (\w+)(?: *= *([a-zA-Z0-9.]+))?/g)) {
						if (!firstArg) {
							methodSignature += ", ";
							methodTableSignature += ", ";
						}
						methodSignature += `${markdownType(arg[1])} ${arg[2]}`;
						methodTableSignature += `${markdownType(arg[1])} ${arg[2]}`;
						if (arg[3]) {
							methodSignature += ` = ${arg[3]}`;
							methodTableSignature += ` = ${arg[3]}`;
						}
						firstArg = false;
					}
					methodSignature += ")";
					methodTableSignature += ")";
	
					if (flags) {
						methodSignature += " " + flags;
						methodTableSignature += " " + flags;
					}
	
					s += `**Signature**: ${methodSignature}`;
					methods.push({
						ret: markdownType(ret),
						signature: methodTableSignature
					});
					return s;
				});
	
				let methodsTable = methods.map(m => {
					return `| ${m.ret} | ${m.signature} |`
				});
	
				markdown = markdown.replace(/\[Methods\]/, () => `| Returns | Method |\n| --- | --- |\n${methodsTable.join("\n")}`);
			}	

			next(markdown);
		});
	};

	// Add plugin to docsify's plugin array
	$docsify = $docsify || {};
	$docsify.plugins = [].concat(apifyPlugin, $docsify.plugins || []);
})();