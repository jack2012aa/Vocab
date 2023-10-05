# Vocab
> Help you to learn new vocabularies.

## Introduce
This is a simple application you can take notes on vocabs. You can later on generate a test to review those vocabs. \
Two types of test can be generated: English to Chinese *e_to_c* and Chinese to English *c_to_e*. In *e_to_c* mode, either answering in Chinese or English synonymouses is acceptable. In *c_to_e* mode, all synonymouses are acceptable.

## Structure
`Word` represents an English word and its Chinese meaning. `Dictionary` contains those words and can be serialized for future using. `Teacher` handles how to produce a test. The [__Boost library__](https://www.boost.org/) is used in serialization.

To relate words with similar meaning, a tree structure is used in `Dictionary`. `Word.synonymouses` is only used as a hint and answer. 

`Dictionary.vec` stores words. `Dictionary.map` stores pointers to words in `Dictionary.vec`. The map helps to access Word faster. 

## Result

## To Do
* Use `Map` to reconstruct `Dictionary.map`.
* Refine the workflow.
* Add methods to edit and delete words.