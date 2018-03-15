'use strict'


window.onload = (function (win, doc) {

	// -------------------------------------------------------------------------
	// Tag production machine
	var verbose = false;
	var loud = false;
	var tagStack = [doc.body]
	var oprStack = [];

	// -------------------------------------------------------------------------
	// example: tagify('p','tag','align','center','attr','msg','text','put');
	const machine = {
		"clear": function () {
			oprStack = [doc.body];
		},
		"tag": function () {
			tagStack.push (doc.createElement(oprStack.pop ()));
		},
		"attr": function () {
			var key = oprStack.pop ();
			var val = oprStack.pop ();
			var tag = tagStack[tagStack.length - 1];
			tag.setAttribute (key, val);
		},
		"text": function () {
			var val = tagStack[tagStack.length - 1];
			val.innerHTML = oprStack.pop ();
		},
		"put": function () {
			var val = tagStack.pop ();
			var tgt = tagStack.pop ();
			tgt.appendChild (val);
			tagStack.push (tgt);
		},
		"loud": function () {
			loud = !loud;
		},
		"check": function () {
			var ok = (tagStack.length == 1 && tagStack[0] == doc.body);
			ok || console.log (
				'stack',
				(tagStack.length < 1) ? 'underflow' : 'garbage');
		},
	}; // machine

	// -------------------------------------------------------------------------
	// TODO try/catch failing operations.
	var tagify = function () {
		loud = false;
		for (var token of arguments) {
			var op = machine[token];
			(loud || verbose) && console.log ("tk:", token, machine[token], op);
			if (op === undefined) {
				(loud || verbose) && console.log('pushing', token);
				oprStack.push (token);
			} else {
				(loud || verbose) && console.log('use operator', op, 'on', token);
				try {
					op (token);
				} catch (err) {
					console.log (err.message);
				}
			}
		}
	}; // tagify

	// -------------------------------------------------------------------------
	function HEREDOC (f) {
		return f.
			toString().
			split('\n').
			slice(1,-1).
			join('\n').
			normalize('NFC');
	} // HEREDOC

	// -------------------------------------------------------------------------
	var grid = function (pattern) {
		verbose && console.log('grid');

		if (typeof pattern == "string") {
			var body = doc.body;
			// TODO Use digits instead of xo and use them as colspans
			// make sure each row adds to the same sum
			const allow = 'xo\n';  // o means open cell, x means extended cell
			// Get rid of <CR>
			if (pattern) {
				pattern.replace('\r\n', '\n');
				pattern.replace('\r', '\n');
				// Check for rejects
				var remains = pattern;
				for (var chr of allow) {
					var re = new RegExp(chr, 'gim');
					verbose && console.log('replacing',chr,'in',remains);
					remains = remains.replace(re, '');
					verbose && console.log('new version:',remains);
				}
				if (remains != '') {
					verbose && console.log('found "' + remains + '"');
					verbose && console.log('patterns chars:', allow);
					return;
				}
				var lines = pattern.split('\n');
				for (var I=lines.length, i=1; i < I; ++i) {
					if (lines[i].length != lines[i-1].length) {
						verbose&& console.log('grid line lengths must be equal');
						return;
					}
				}
				for (var I=lines.length, i=0; i < I; ++i) {
				}
				verbose && console.log(lines);
			}
		}

		var ABC123 = {
			A1:'logo' , A2:'title', A3:'meta' ,
			B1:'nav'  , B2:'body' , B3:'aside',
			C1:'blank', C2:'legal', C3:'special',
		};
		var table = doc.createElement ('table');
		table.style.width  = '100%';
		table.style.height = '100%';
		verbose && console.log ('grid');
		for (var letter of 'ABC') {
			var tr = doc.createElement ('tr');
			for (var digit of '123') {
				var id = letter + digit;
				var td = doc.createElement ('td');
				td.innerHTML = ABC123[id];
				td.setAttribute ('id', 'grid_' + id);
				var B2 = (id == 'B2')
				var width  =  B2 ? '100%' : '1%';
				var height =  B2 ? '100%' : '1%';
				td.style.width  = width;
				td.style.height = height;
				tr.appendChild (td);
			}
			table.appendChild (tr);
		}
		body.appendChild (table);
	} // grid

	// -------------------------------------------------------------------------
	var quiki = function () {
		const fmt   = '<hr /><small><small>[{}]</small></small><br />';
		var body    = doc.body;
		var head    = doc.head;
		var quikis  = doc.getElementsByClassName ('quiki');
		var sources = [];
		for (var quiki of quikis) {
			if (quiki.classList.contains ('markdown')) {
				console.log (quiki);
				sources.push (quiki);
			}
		}

		grid ('oxx\nooo\noxx')

		// markdown conversion
		for (var source of sources) {
			var title    = source.id ? fmt.replace ('{}', source.id) : "";
			var markdown = source.innerHTML;
			var content  = doc.jlettvin.wiki.markdown (markdown);
			tagify ('span', 'tag', title + content, 'text', 'put', 'check');
		}

		{ // reload button
			tagify (
				'loud',
				'button', 'tag',
				'reload', 'text',
				'location.reload(true)', 'onclick', 'attr',
				'put',
				'check',
			);
		}

		{ // JDL abbr to show tooltip
			tagify('br', 'tag', 'put', 'check');
			tagify (
				'abbr', 'tag',
				'Jonathan D. Lettvin', 'title', 'attr',
				'JDL', 'text',
				'put',
				'check',
			);
		}
	} // quiki

	// -------------------------------------------------------------------------
	var loadJS = function (url, fun, loc) {
		var script                = doc.createElement ('script');
		script.src                = url;
		script.onload             = fun;
		script.onreadystatechange = fun;
		loc.appendChild           (script);
	} // loadJS

	// -------------------------------------------------------------------------
	var loadedCall = function () { quiki (); }

	// -------------------------------------------------------------------------
	// Load all the modules needed to transform markdown to HTML
	loadJS ('js/wiki.js', loadedCall, doc.body);

})(window, document);
