test_that("insert method works", {
    system("beectl eval example cleanup.lua")
    system("beectl eval example init.lua")

    beer <- new(Bee)
    expect_that(beer$ping(), is_true())

    res <- beer$insert("test", list(1L, 1001L, "A", T, F, list("a", 1L, 1.0, list(1, 2, list("a", "b"), 3))))
    expect_that(res[[1]], equals(list(1L, 1001L, "A", TRUE, FALSE, list("a", 1L, 1.0, list(1, 2, list("a", "b"), 3)))))

    res <- beer$insert("test", list(2L, 3.14, "B", T, F, list(1, 2, 3)))
    expect_that(res[[1]], equals(list(2L, 3.14, "B", T, F, list(1, 2, 3))))

    res <- beer$insert("test", list(3L, NULL, NULL))
    expect_that(res[[1]], equals(list(3L, NULL, NULL)))

    res <- beer$select("test", 3L, NULL)
    expect_that(length(res), equals(1))
    expect_that(res[[1]], equals(list(3L, NULL, NULL)))

    expect_that(beer$insert("test", list(1L, NULL, NULL)), throws_error())

    system("beectl eval example cleanup.lua")
})

test_that("insert method fails on unsupported R's data types", {
    system("beectl eval example cleanup.lua")
    system("beectl eval example init.lua")

    beer <- new(Bee)
    expect_that(beer$ping(), is_true())

    # We don't (yet) support factors' serialization
    testthat::expect_error(beer$insert("test", list(2000L, sapply(list("a", "b"), as.factor))))

    # don't support functions
    f <- function(x) {
        return (x*x)
    }
    testthat::expect_error(beer$insert("test", list(2000L, f)))

    system("beectl eval example cleanup.lua")
})

test_that("insert method works with two indexes", {
    system("beectl eval example cleanup.lua")
    system("beectl eval example init.lua")

    beer <- new(Bee)
    testthat::expect_true(beer$ping())

    res <- beer$insert("test2", list(1L, "AA", T, F, list("a", 1L, 1.0, list(1, 2, list("a", "b"), 3))))
    testthat::expect_equal(res[[1]], list(1L, "AA", TRUE, FALSE, list("a", 1L, 1.0, list(1, 2, list("a", "b"), 3))))

    res <- beer$insert("test2", list(2L, "AA", 3.14))
    testthat::expect_equal(res[[1]], list(2L, "AA", 3.14))

    system("beectl eval example cleanup.lua")
})

test_that("insert method fails on data which doesn't fit index", {
    system("beectl eval example cleanup.lua")
    system("beectl eval example init.lua")

    beer <- new(Bee)
    expect_that(beer$ping(), is_true())

    # "test2" namespace has two indexes and the second one is on STR data type
    testthat::expect_error(beer$insert("test2", list(2000L, 20)))

    system("beectl eval example cleanup.lua")
})
