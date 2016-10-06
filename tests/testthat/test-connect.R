test_that("Bee class ctor connects", {
    beer <- new(Bee)
    expect_that(beer$ping(), is_true())
})

test_that("Bee class ctor fails", {
    expect_that(beer <- new(Bee, "localhost", 33333), throws_error())
})