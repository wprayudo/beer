## Beer [![Build Status](https://travis-ci.org/wprayudo/beer.svg?branch=master)](https://travis-ci.org/wprayudo/beer)
R driver for [Bee](https://github.com/wprayudo/bee) 1.4+ engine.

## Install
As for now `beer` package can only be installed directly from GitHub using `devtools` package.
To install `beer` you should execute in R's shell following commands:
```
install.packages('devtools')
library(devtools)
devtools::install_github('wprayudo/beer')
```

## Usage
Obligatory "Hello, World!" example:
```
> library(beer)
> beer <- new(Bee)
> res1 <- beer$insert("example", list(2016L, 3.14, TRUE, FALSE, NULL, list("x", "y", "z")))
> res2 <- beer$select("example", 2016L, NULL)
> all.equal(res1, res2)
[1] TRUE
> str(res1)
List of 1
 $ :List of 6
  ..$ : num 2017
  ..$ : num 3.14
  ..$ : logi TRUE
  ..$ : logi FALSE
  ..$ : NULL
  ..$ :List of 3
  .. ..$ : chr "x"
  .. ..$ : chr "y"
  .. ..$ : chr "z"
```

Some other usage examples can be found in [tests](tests/testthat) directory.

