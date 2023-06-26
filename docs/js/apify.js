const GODOT_VERSION = "4.0";

const CLASS_REFERENCE = {
	"UI": "api/ui.md",
	"MotionRef": "api/motion_ref.md"
};

const linkClass = (type) => {
	return CLASS_REFERENCE[type] || (
		type == "void" ? "void" : 
		`https://docs.godotengine.org/en/${GODOT_VERSION}/classes/class_${type.toLowerCase()}.html`
	);
}

const tagLinkClass = (type) => `[${type}](${linkClass(type)})`;

(function () {
	var apifyPlugin = function (hook, vm) {
		hook.beforeEach(function (markdown, next) {
			let parseAPI = false;
			
			markdown = markdown.replace(/\<\w+ parse-class-api\/>/, () => {
				parseAPI = true;
				return "";
			});

			markdown = markdown.replace(/<\w+ class-ref>(\w+)<\/\w+>/g, (s, className) => `[${className}](${linkClass(className)})`);

			if (parseAPI) {
				let methods = [];
				let properties = [];
				
				// Extract methods
				markdown = markdown.replace(/<\w+ class-method>(\w+) +(\w+) *\(([\w ,.="]*)\) *([\w ]+)?<\/\w+>/g, (s, ret, name, args, flags) => {
					s = `### ${name}\n`;
	
					s += `**Signature**: ${tagLinkClass(ret)} ${name} (`;
					let methodTableSignature = `[${name}](#${name}) (`;
					let firstArg = true;
					for (const arg of args.matchAll(/(\w+) (\w+)(?: *= *([a-zA-Z0-9.]+))?/g)) {
						if (!firstArg) {
							s += ", ";
							methodTableSignature += ", ";
						}
						s += `${tagLinkClass(arg[1])} ${arg[2]}`;
						methodTableSignature += `${tagLinkClass(arg[1])} ${arg[2]}`;
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
						ret: tagLinkClass(ret),
						signature: methodTableSignature
					});
					return s;
				});

				// Extract properties
				markdown = markdown.replace(/<\w+ class-property *(readonly)?>(\w+) +(\w+) *=? *([\w ,.="]+)?<\/\w+>/g, (s, readonly, type, name, defVal) => {
					s = `### ${name}\n`;

					s += `**Signature**: ${tagLinkClass(type)} ${name}`;
					if (defVal != null)
						s += ` = \`${defVal}\`\n`;
					
					if (!readonly) {
						s += `- void set_${name}( ${tagLinkClass(type)} value )\n`;
					}
					s += `- ${tagLinkClass(type)} get_${name}( )`;

					properties.push({
						type: tagLinkClass(type),
						name: `[${name}](#${name})`,
						defVal
					});
					
					return s;
				})

				markdown = markdown.replace(/<\w+ class-member-ref>(\w+)<\/\w+>/g, (s, name) => {
					return `[${name}](#${name})`
				});
	
				let methodsTable = methods.map(m => {
					return `| ${m.ret} | ${m.signature} |`
				});

				let propertiesTable = properties.map(p => {
					let defVal = p.defVal != null ? `\`${p.defVal}\`` : " ";
					return `| ${p.type} | ${p.name} | ${defVal} |`
				});
	
				markdown = markdown.replace(/<\w+ class-methods>.*<\/\w+>/, () => `| Returns | Method |\n| --- | --- |\n${methodsTable.join("\n")}`);
				markdown = markdown.replace(/<\w+ class-properties>.*<\/\w+>/, () => `| Type | Property | Default |\n| --- | --- | --- |\n${propertiesTable.join("\n")}`);
			}

			next(markdown);
		});
	};

	// Add plugin to docsify's plugin array
	$docsify = $docsify || {};
	$docsify.plugins = [].concat(apifyPlugin, $docsify.plugins || []);
})();