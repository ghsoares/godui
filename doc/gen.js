import fs from 'fs';
import path from 'path';
import { parseStringPromise } from 'xml2js';

const GODOT_VERSION = "4.1";

const CLASS_REFERENCE = {
	"UI": "#api/ui.md",
	"MotionRef": "#api/motionref.md",
	"DrawRef": "#api/drawref.md"
};

const OUTPUT_PATH = "../web-documentation/api/";

const classRefLink = (className) => {
	if (CLASS_REFERENCE[className]) {
		return CLASS_REFERENCE[className];
	}
	return `https://docs.godotengine.org/en/${GODOT_VERSION}/classes/class_${className.toLowerCase()}.html`;
}

const classConstantRefLink = (className, cons) => {
	cons = cons.replace(/_/g, "-").toLowerCase();
	let classLink = classRefLink(className);
	if (CLASS_REFERENCE[className])
		return classLink + `?id=const-${cons}`;
	else
		return classLink + `#class-${className.toLowerCase()}-constant-${cons}`;
}

const classEnumRefLink = (className, enm) => {
	enm = enm.replace(/_/g, "-").toLowerCase();
	let classLink = classRefLink(className);
	if (CLASS_REFERENCE[className])
		return classLink + `?id=enum-${enm}`;
	else
		return classLink + `#class-${className.toLowerCase()}-enum-${enm}`;
}

const classMemberRefLink = (className, member) => {
	member = member.replace(/_/g, "-").toLowerCase();
	let classLink = classRefLink(className);
	if (CLASS_REFERENCE[className])
		return classLink + `?id=property-${member}`;
	else
		return classLink + `#class-${className.toLowerCase()}-property-${member}`;
}

const classMethodRefLink = (className, method) => {
	method = method.replace(/_/g, "-").toLowerCase();
	let classLink = classRefLink(className);
	if (CLASS_REFERENCE[className])
		return classLink + `?id=method-${method}`;
	else
		return classLink + `#class-${className.toLowerCase()}-method-${method}`;
}

const classRefAnchor = (className) => {
	if (className == "void") return "void";
	return `[${className}](${classRefLink(className)})`;
}

const classConstantRefAnchor = (className, cons) => {
	return `[${className}.${cons}](${classConstantRefLink(className, cons)})`;
}

const classEnumRefAnchor = (className, enm) => {
	return `[${className}.${enm}](${classEnumRefLink(className, enm)})`;
}

const classMemberRefAnchor = (className, member) => {
	return `[${className}.${member}](${classMemberRefLink(className, member)})`;
}

const classMethodRefAnchor = (className, method) => {
	return `[${className}.${method}](${classMethodRefLink(className, method)})`;
}

const parseBBCode = (s = "") => {
	const parseBoldStart = (s = "") => {
		if (!s.startsWith("[b]")) return null;
		return {
			type: "tag_bold_start",
			next: s.slice(3)
		};
	}

	const parseBoldEnd = (s = "") => {
		if (!s.startsWith("[/b]")) return null;
		return {
			type: "tag_bold_end",
			next: s.slice(4)
		};
	}

	const parseCodeStart = (s = "") => {
		if (!s.startsWith("[code]")) return null;
		return {
			type: "tag_code_start",
			next: s.slice(6)
		};
	}

	const parseCodeEnd = (s = "") => {
		if (!s.startsWith("[/code]")) return null;
		return {
			type: "tag_code_end",
			next: s.slice(7)
		};
	}

	const parseConstant = (s = "") => {
		let tag = s.match(/^\[constant ([\w.]+)\]/);
		if (!tag) return null;
		return {
			type: "constant",
			value: tag[1],
			next: s.slice(tag[0].length)
		}
	}

	const parseEnum = (s = "") => {
		let tag = s.match(/^\[enum ([\w.]+)\]/);
		if (!tag) return null;
		return {
			type: "enum",
			value: tag[1],
			next: s.slice(tag[0].length)
		}
	}

	const parseMember = (s = "") => {
		let tag = s.match(/^\[member ([\w.]+)\]/);
		if (!tag) return null;
		return {
			type: "member",
			value: tag[1],
			next: s.slice(tag[0].length)
		}
	}

	const parseMethod = (s = "") => {
		let tag = s.match(/^\[method ([\w.]+)\]/);
		if (!tag) return null;
		return {
			type: "method",
			value: tag[1],
			next: s.slice(tag[0].length)
		}
	}

	const parseParam = (s = "") => {
		let tag = s.match(/^\[param ([\w.]+)\]/);
		if (!tag) return null;
		return {
			type: "param",
			value: tag[1],
			next: s.slice(tag[0].length)
		}
	}

	const parseClassReference = (s = "") => {
		let tag = s.match(/^\[(\w+)\]/);
		if (!tag) return null;
		return {
			type: "class",
			value: tag[1],
			next: s.slice(tag[0].length)
		}
	}

	const parseTag = (s = "") => {
		const start = parseBoldStart(s) || parseCodeStart(s) || parseConstant(s) || parseEnum(s) || parseMember(s) || parseMethod(s) || parseParam(s) || parseClassReference(s);
		if (!start) return null;
		if (start.type === "tag_bold_start") {
			let text = parseText(start.next);
			const end = parseBoldEnd(text?.next || start.next);
			if (!end) return null;
			return {
				type: "bold",
				value: text,
				next: end.next
			}
		} else if (start.type === "tag_code_start") {
			let text = parseText(start.next);
			const end = parseCodeEnd(text?.next || start.next);
			if (!end) return null;
			return {
				type: "code",
				value: text,
				next: end.next
			}
		}
		return start;
	}

	const parseText = (s = "") => {
		let text = "";

		while (s) {
			if (s[0] == "[") {
				if (s[1] == "[") {
					text += "[";
					s = s.slice(2);
					continue;
				}
				if (text) {
					return {
						type: "text",
						value: text,
						next: s
					}
				}
			}
			text += s[0];
			s = s.slice(1);
		}

		if (!text) return null;

		return {
			type: "text",
			value: text,
			next: s
		}
	}

	const syntaxBBCode = (s = "") => {
		let bbcode = [];
		let prevLen = s.length;

		while (s) {
			const content = parseTag(s) || parseText(s);
			if (!content) break;
			bbcode.push(content);
			s = content.next;
			if (s.length === prevLen) {
				console.warn("Infinite recursion");
				break;
			}
			prevLen = s.length;
		}

		return {
			type: "bbcode",
			value: bbcode
		};
	}

	const parseBBCode = (syntax) => {
		switch (syntax.type) {
			case "bbcode": {
				let parsed = "";
				for (const v of syntax.value) {
					parsed += parseBBCode(v);
				}
				return parsed;
			};
			case "text": return syntax.value;
			case "bold": return "**" + parseBBCode(syntax.value) + "**";
			case "code": return `<code class="code-gdscript">` + parseBBCode(syntax.value) + `</code>`;
			case "constant": {
				const classRef = syntax.value.match(/^(\w+)\./);
				let cons = syntax.value;
				if (classRef) {
					cons = cons.slice(classRef[0].length);
					return classConstantRefAnchor(classRef[1], cons);
				}
				return `[${cons}](#const-${cons.toLowerCase()})`;
			}
			case "enum": {
				const classRef = syntax.value.match(/^(\w+)\./);
				let enm = syntax.value;
				if (classRef) {
					enm = enm.slice(classRef[0].length);
					return classEnumRefAnchor(classRef[1], enm);
				}
				return `[${enm}](#enum-${enm.toLowerCase()})`;
			}
			case "member": {
				const classRef = syntax.value.match(/^(\w+)\./);
				let member = syntax.value;
				if (classRef) {
					member = member.slice(classRef[0].length);
					return classMemberRefAnchor(classRef[1], member);
				}
				return `[${member}](#property-${member.toLowerCase()})`;
			}
			case "method": {
				const classRef = syntax.value.match(/^(\w+)\./);
				let method = syntax.value;
				if (classRef) {
					method = method.slice(classRef[0].length);
					return classMethodRefAnchor(classRef[1], method);
				}
				return `[${method}](#method-${method.toLowerCase()})`;
			}
			case "param": return "`" + syntax.value + "`";
			case "class": return classRefAnchor(syntax.value);
		}
	}

	const syntax = syntaxBBCode(s);
	return parseBBCode(syntax);
}

const parseClassReference = (xml) => {
	return new Promise((resolve, reject) => {
		const $class = xml.class;
		const classProps = $class.$;
		const name = classProps.name;
		const inherits = classProps.inherits;
		const briefDescription = parseBBCode(
			$class.brief_description?.at(0)?.replace(/\t/g, "").trim()
		);
		const description = parseBBCode(
			$class.description?.at(0)?.replace(/\t/g, "").trim()
		);
		const members = $class.members?.at(0)?.member.map(m => {
			const memberType = m.$.type;
			const memberName = m.$.name;
			const memberSetter = m.$.setter;
			const memberGetter = m.$.getter;
			const memberDescription = parseBBCode(
				m._?.replace(/\t/g, "").trim()
			);
			return {
				type: memberType,
				name: memberName,
				setter: memberSetter,
				getter: memberGetter,
				description: memberDescription
			}
		});
		const methods = $class.methods?.at(0)?.method.map(m => {
			const methodReturn = m.return[0].$.type;
			const methodName = m.$.name;
			const methodQualifiers = m.$.qualifiers;
			const methodParams = m.param?.map(p => {
				p = p.$;
				const paramIndex = p.index;
				const paramType = p.type;
				const paramName = p.name;
				const paramDefault = p.default;
				return {
					index: Number(paramIndex),
					type: paramType,
					name: paramName,
					default: paramDefault
				};
			}).sort((a, b) => a - b).map(p => {
				delete p.index;
				return p;
			});
			const methodDescription = parseBBCode(
				m.description?.at(0)?.replace(/\t/g, "").trim()
			);
			return {
				return: methodReturn,
				name: methodName,
				qualifiers: methodQualifiers,
				params: methodParams,
				description: methodDescription
			};
		});

		let html = "<!-- Generated by document generator, changes will be lost. -->\n\n";

		html += `# ${name}\n\n`;
		html += `**Inherits** ${classRefAnchor(inherits)}\n\n`;
		if (briefDescription)
			html += `${briefDescription}\n\n`;

		html += `## Description\n\n${description}\n\n`;

		if (members) {
			html += `## Properties\n\n`;
			html += "| Type | Name | Default |\n";
			html += "| --- | --- | --- |\n";

			members.forEach(m => {
				html += `| ${classRefAnchor(m.type)} `;
				html += `| [${m.name}](#property-${m.name}) `;
				if (m.default) {
					html += `| <code class="code-gdscript">${m.default}</code>`
				} else {
					html += "|";
				}
				html += " |\n";
			});
		}

		if (methods) {
			html += `## Methods\n\n`;
			html += "| Return | Method |\n";
			html += "| --- | --- |\n";

			methods.forEach(m => {
				html += `| ${classRefAnchor(m.return)} `;
				html += `| [${m.name}](#method-${m.name}) ( `;
				m.params?.forEach((p, i) => {
					html += `${classRefAnchor(p.type)} ${p.name}`;
					if (p.default) html += ` = <code class="code-gdscript">${p.default}</code>`;
					if (i < m.params.length - 1) html += ",";
					html += " ";
				});
				html += ")";
				if (m.qualifiers) {
					html += ` ${m.qualifiers}`;
				}
				html += " |\n";
			});

			html += "\n";
		}

		if (members) {
			html += "## Property descriptions\n\n";

			members.forEach(m => {
				html += `### <a id="property-${m.name}">${classRefAnchor(m.type)} [${m.name}](#property-${m.name}) `;
				if (m.default) {
					html += ` = <code class="code-gdscript">${m.default}</code>`;
				}
				html += "</a>\n\n";

				if (m.setter) {
					html += `> - <span style="opacity: 0.8">void ${m.setter} ( ${classRefAnchor(m.type)} value )</span>\n`;
				}
				if (m.getter) {
					html += `> - <span style="opacity: 0.8">${classRefAnchor(m.type)} ${m.getter} ( )</span>\n`;
				}
				html += "> \n";

				if (m.description) {
					html += "> " + m.description.replace(/\n/g, "\n> ");
				} else {
					html += "> Missing description";
				}
				html += "\n\n";
			});
		}

		if (methods) {
			html += "## Method descriptions\n\n";

			methods.forEach(m => {
				html += `### <a id="method-${m.name}">${classRefAnchor(m.return)} [${m.name}](#method-${m.name}) ( `;

				m.params?.forEach((p, i) => {
					html += `${classRefAnchor(p.type)} ${p.name}`;
					if (p.default) html += ` = <code class="code-gdscript">${p.default}</code>`;
					if (i < m.params.length - 1) html += ",";
					html += " ";
				});
				html += ")";
				if (m.qualifiers) {
					html += ` ${m.qualifiers}`;
				}
				html += "</a>\n\n";

				if (m.description) {
					html += "> " + m.description.replace(/\n/g, "\n> ");
				} else {
					html += "> Missing description";
				}

				html += "\n\n";
			});
		}

		resolve(html);
	});
}

const parseFile = (filePath, filename) => {
	fs.readFile(filePath, 'utf-8', (err, data) => {
		if (err) {
			console.error(err);
			return;
		}
		const className = path.parse(filename).name;
		parseStringPromise(data)
			.then(xml => parseClassReference(xml))
			.then(html => new Promise((resolve, reject) => {
				fs.writeFile(path.join(OUTPUT_PATH, `${className.toLowerCase()}.md`), html, (err) => {
					if (err) reject(err);
					else resolve();
				})
			}))
			.catch(err => console.error(err));
	});
};

fs.readdir('./classes', (err, files) => {
	if (err) {
		console.error(err);
		return;
	}
	files.forEach(f => {
		parseFile(path.join('./classes', f), f);
	});
});