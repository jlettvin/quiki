# quiki
Customizable client-side wiki markdown

* index.html: minimalistic use case
* serve.sh: localhost quiki server
* img/quiki_tm.png: favicon
* js/quiki.js: quiki manager
* js/wiki.js: temporary quiki-to-HTML interpreter
* fsm/&x2a;: Finite State Machine in C++ for conversion to js

This library converts markdown content
delivered to a client in the HTML &lt;head&gt;
to HTML using javascript code.

This link offers a view of the example conversion:
https://rawgit.com/jlettvin/quiki/master/index.html

This is a work in progress, so do not expect
all the bells and whistles yet.
Currently a simple hand-written markdown is used.

TODO:
you get to either use the default markdown syntax
or you can create either modifications or entire
new syntax to drive the content creation API.

