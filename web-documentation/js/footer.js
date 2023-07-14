(function () {
	var footerPlugin = function (hook, vm) {
		hook.beforeEach(function (html) {
			return (
				html +
				'\n\n----\n\n' +
				'<a href="https://docsify.js.org" target="_blank" style="color: inherit; font-weight: normal; text-decoration: none;">Powered by docsify</a>'
			);
		});
	};

	// Add plugin to docsify's plugin array
	$docsify = $docsify || {};
	$docsify.plugins = [].concat(footerPlugin, $docsify.plugins || []);
})();