
Robinson-C
==========

C implementation of Matt Brubeck's "[Robinson][]" HTML engine.

It uses this [bitmap library][bitmap] to render graphics.

[Robinson]: https://limpet.net/mbrubeck/2014/08/08/toy-layout-engine-1.html
[bitmap]: https://github.com/wernsey/bitmap

References
----------

1. [Let's build a browser engine!][Robinson], Matt Brubeck
2. From the W3C specification for CSS:
 * [Visual formatting model](https://www.w3.org/TR/CSS2/visuren.html)
 * [Box model](https://www.w3.org/TR/CSS2/box.html)
 * [Syntax and basic data
   types](https://www.w3.org/TR/2011/REC-CSS2-20110607/syndata.html)
3. [How Browsers
  Work:](https://www.html5rocks.com/en/tutorials/internals/howbrowserswork/)
  _Behind the scenes of modern web browsers_, by Tali Garsiel and Paul Irish
4. [Understanding the CSS box model for inline
  elements](https://hacks.mozilla.org/2015/03/understanding-inline-box-model/),
  Patrick Brosset, Mozilla
5. [Quantum Up Close: What is a browser
  engine?](https://hacks.mozilla.org/2017/05/quantum-up-close-what-is-a-browser-engine/),
  Mozilla

Other implementations:
1. [WebWhir](https://github.com/reesmichael1/WebWhir)
1. <https://github.com/gooofy/robinson> is an implementation in Python that
  does support text and inline elements (GPL).

TODO
----

Organised according to priority and (_apparent_) ease of implementation

* [ ] Text rendering, obviously.
* [ ] Inline elements. To implement, start at the [Normal Flow][normalflow]
  section of (2).
  * Lines that do not fit in the width of a container should be split into
    multiple `inline` blocks.
  * Note what happens to the margin and especially the _border_.
* [ ] [Absolute
  positioning](https://www.w3.org/TR/CSS2/visuren.html#absolute-positioning)
* [ ] [Collapsing
  Margins](https://www.w3.org/TR/CSS2/box.html#collapsing-margins)
* [ ] Image support - might depend on the framework I use to render it
* [ ] `style` attribute, and `<style/>` tag
* [ ] More sophisticated CSS parsing as per [Syntax and basic data
  types](https://www.w3.org/TR/2011/REC-CSS2-20110607/syndata.html)

[normalflow]: https://www.w3.org/TR/CSS2/visuren.html#normal-flow

License
-------

MIT License. See the file [LICENSE](LICENSE) for details.
