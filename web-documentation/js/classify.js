const GODOT_VERSION = "4.0";

const CLASS_REFERENCE = {
	"UI": "api/ui.md",
	"MotionRef": "api/motion_ref.md",
	"DrawRef": "api/draw_ref.md"
};

const createLink = (label, link, newWindow = true) => {
	let a = link ?
		$(`<a href="${link}">${label}</a>`) :
		$(`<a>${label}</a>`);
	
	if (newWindow) {
		a.attr("target", "_blank");
		a.attr("rel", "noopener");
	}
	
	return a;
}

const linkClass = (type) => {
	let link = CLASS_REFERENCE[type];
	return link ? `#/${link}` : (
		type == "void" ? "void" : 
		`https://docs.godotengine.org/en/${GODOT_VERSION}/classes/class_${type.toLowerCase()}.html`
	);
}

const anchorLinkClass = (type) => {
	let link = linkClass(type);
	if (link !== type) {
		return createLink(type, link, !link.startsWith("#/")).addClass("class-ref-anchor");
	} else {
		return createLink(type, null, false).addClass("class-ref-anchor");
	}
}

(function () {
	var apifyPlugin = function (hook, vm) {
		hook.afterEach(function (html, next) {
			let dom = new DOMParser().parseFromString(html, "text/html").body;

			dom.querySelectorAll("code.code-gdscript").forEach((el) => {
				el.innerHTML = Prism.highlight(el.textContent, Prism.languages.gdscript, 'gdscript');
				el.classList.add("language-gdscript");
				el.classList.remove("code-gdscript");
			});

			// let parseAPI = false;

			// dom.querySelectorAll("[parse-class-api]").forEach((el) => {
			// 	parseAPI = true;
			// 	el.remove();
			// });

			// dom.querySelectorAll("[class-ref]").forEach((el) => {
			// 	let className = el.innerText.trim();
			// 	$(el).replaceWith(anchorLinkClass(className));
			// });

			// if (parseAPI) {
			// 	let properties = [];
			// 	let methods = [];

			// 	dom.querySelectorAll("[class-property]").forEach((el) => {
			// 		let syntax = el.innerText.trim();
			// 		let syntaxMatch = syntax.match(/^(\w+) +(\w+) *=? *([\w ,.="\{\}\[\]\(\)]+)?$/);
			// 		if (syntaxMatch == null) throw new SyntaxError("Wrong syntax for 'class-property' tag");
			// 		let [_, propType, propName, propDef] = syntaxMatch;
			// 		let readonly = el.hasAttribute("readonly");
					
			// 		let prop = {
			// 			type: propType,
			// 			name: propName,
			// 			default: propDef,
			// 			readonly
			// 		};

			// 		let propEl = $(`<h3 class="class-property" id="prop-${propName}"></h3>`);
			// 		propEl.append(anchorLinkClass(propType));
			// 		propEl.append(" ");
			// 		propEl.append(createLink(propName, `#/${vm.route.file}?id=prop-${propName}`, false).addClass("class-property-name"));
			// 		if (propDef !== undefined) {
			// 			propEl.append(" = ");
			// 			propEl.append($(Prism.highlight(propDef, Prism.languages.gdscript, 'gdscript')));
			// 		}
			// 		let list = $(`<ul class="class-method class-property-getter-setter"></ul>`);
			// 		if (!readonly) {
			// 			list.append(
			// 				$("<li></li>")
			// 					.append(anchorLinkClass("void"))
			// 					.append(` `)
			// 					.append(`<span class="class-method-name">set_${propName}</span>`)
			// 					.append(" ( ")
			// 					.append(anchorLinkClass(propType))
			// 					.append(" ")
			// 					.append($(`<span class="class-method-argument-name">value</span>`))
			// 					.append(" )")
			// 			);
			// 		}
			// 		list.append(
			// 			$("<li></li>")
			// 				.append(anchorLinkClass(propType))
			// 				.append(" ")
			// 				.append(`<span class="class-method-name">get_${propName}</span>`)
			// 				.append(` ( )`)
			// 		);

			// 		$(el).replaceWith([propEl, list]);

			// 		properties.push(prop);
			// 	});

			// 	dom.querySelectorAll("[class-method]").forEach(el => {
			// 		let syntax = el.innerText.trim();
			// 		let syntaxMatch = syntax.match(/^(\w+) +(\w+) *\(([\w ,.="]*)\) *([\w ]+)?$/);
			// 		if (syntaxMatch == null) throw new SyntaxError("Wrong syntax for 'class-method' tag");
			// 		let [_, methodReturn, methodName, methodArguments, methodFlags] = syntaxMatch;
					
			// 		methodArguments = [...methodArguments.matchAll(/(\w+) (\w+) *=? *([a-zA-Z0-9.]+)?/g)].map(arg => {
			// 			return {
			// 				type: arg[1],
			// 				name: arg[2],
			// 				default: arg[3]
			// 			}
			// 		});

			// 		methodFlags = (methodFlags || "").split(/ +/g).map(v => v.trim());

			// 		let method = {
			// 			returns: methodReturn,
			// 			name: methodName,
			// 			arguments: methodArguments,
			// 			flags: methodFlags
			// 		};

			// 		let methodEl = $(`<h3 class="class-method" id="method-${methodName}"></h3>`);
			// 		methodEl.append(anchorLinkClass(methodReturn));
			// 		methodEl.append(" ");
			// 		methodEl.append(createLink(methodName, `#/${vm.route.file}?id=method-${methodName}`, false).addClass("class-method-name"));
			// 		methodEl.append(" ( ");
			// 		if (methodArguments.length > 0) {
			// 			let first = true;
			// 			for (const arg of methodArguments) {
			// 				if (!first) methodEl.append(", ");
			// 				methodEl.append(anchorLinkClass(arg.type));
			// 				methodEl.append(" ");
			// 				methodEl.append($(`<span class="class-method-argument-name">${arg.name}</span>`))
			// 				if (arg.default !== undefined) {
			// 					methodEl.append(" = ");
			// 					methodEl.append($(Prism.highlight(arg.default, Prism.languages.gdscript, 'gdscript')));
			// 				}
			// 				first = false;
			// 			}
			// 			methodEl.append(" ");
			// 		}
			// 		methodEl.append(")");
			// 		if (methodFlags.length > 0) {
			// 			methodEl.append(" " + methodFlags.join(" "));
			// 		}

			// 		$(el).replaceWith(methodEl);

			// 		methods.push(method);
			// 	});

			// 	dom.querySelectorAll("[class-properties]").forEach(el => {
			// 		let tableEl = $(`<table class="class-properties"></table>`);

			// 		for (const prop of properties) {
			// 			tableEl.append(
			// 				$(`<tr class="class-property-row"></tr>`).append(
			// 					$(`<td class="class-property-type"></td>`).append(anchorLinkClass(prop.type)),
			// 					$(`<td class="class-property-name"></td>`).append(createLink(prop.name, `#/${vm.route.file}?id=prop-${prop.name}`, false)),
			// 					$(`<td class="class-property-default"></td>`).append(prop.default === undefined ? "" : $(Prism.highlight(prop.default, Prism.languages.gdscript, 'gdscript')))
			// 				)
			// 			);
			// 		}
					
			// 		$(el).replaceWith(tableEl);
			// 	});

			// 	dom.querySelectorAll("[class-methods]").forEach(el => { 
			// 		let tableEl = $(`<table class="class-methods"></table>`);

			// 		for (const method of methods) {
			// 			let signature = $(`<td class="class-method"></td>`);
			// 			signature
			// 				.append(createLink(method.name, `#/${vm.route.file}?id=method-${method.name}`, false).addClass("class-method-name"))
			// 				.append(" ( ");
						
			// 			if (method.arguments.length > 0) {
			// 				let first = true;
			// 				for (const arg of method.arguments) {
			// 					if (!first) signature.append(", ");
			// 					signature.append($(anchorLinkClass(arg.type)));
			// 					signature.append(" ");
			// 					signature.append(`<span class="class-method-argument-name">${arg.name}</span>`);
			// 					if (arg.default !== undefined) {
			// 						signature.append(" = ");
			// 						signature.append($(Prism.highlight(arg.default, Prism.languages.gdscript, 'gdscript')));
			// 					}
			// 					first = false;
			// 				}
			// 				signature.append(" ");
			// 			}
			// 			signature.append(")");
			// 			if (method.flags.length > 0) {
			// 				signature.append(" " + method.flags.join(" "));
			// 			}
						
			// 			tableEl.append(
			// 				$(`<tr class="class-method-row"></tr>`).append(
			// 					$(`<td class="class-method-returns"></td>`).append(anchorLinkClass(method.returns)),
			// 					signature
			// 				)
			// 			);
			// 		}
					
			// 		$(el).replaceWith(tableEl);
			// 	});

			// 	dom.querySelectorAll("[class-prop-ref]").forEach(el => { 
			// 		let member = el.innerText.trim();

			// 		el.replaceWith(`[${member}](#prop-${member})`);
			// 	});

			// 	dom.querySelectorAll("[class-method-ref]").forEach(el => { 
			// 		let member = el.innerText.trim();

			// 		el.replaceWith(`[${member}](#method-${member})`);
			// 	});
			// }
			next(dom.innerHTML);
		});
	};

	// Add plugin to docsify's plugin array
	$docsify = $docsify || {};
	$docsify.plugins = [].concat(apifyPlugin, $docsify.plugins || []);
})();