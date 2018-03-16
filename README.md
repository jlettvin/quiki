# quiki
Customizable client-side wiki markdown

* index.html: minimalistic use case
* test.html: example markdown conversions
* serve.sh: localhost quiki server
* img/quiki_tm.png: favicon
* js/quiki.js: quiki code
* js/wiki.js: temporary slow quiki-to-HTML interpreter
* fsm/&x2a;: Finite State Machine in C++ for conversion to js

This javascript library generates HTML from markdown content
delivered to a client in the HTML &lt;head&gt; tag.

These two links offer a view of the example conversion:
https://rawgit.com/jlettvin/quiki/master/index.html
https://rawgit.com/jlettvin/quiki/master/test.html

This is a work in progress, so do not expect
all the bells and whistles yet.
Currently a slow buggy markdown interpreter is used.

TODO:
* Make the syntax customizable
* Make a content creation API for customizable syntax
* Write the new interpreter as a Finite State Machine
