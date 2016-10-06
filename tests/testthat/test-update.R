test_that("update method works", {
    system("beectl eval example cleanup.lua")
    system("beectl eval example init.lua")
    system("beectl eval example populate_db2.lua")

    beer <- new(Bee)
    expect_that(beer$ping(), is_true())

    res <- beer$update("test", list(1L), list(index=0L, ops=list(list(field=1, op="+", arg=1L))))
    expect_that(length(res), equals(1))
    expect_that(res[[1]], equals(list(1, 2, 1)))

    res <- beer$update("test", list(2L), list(index=0L, ops=list(list(field=1, op="-", arg=1L))))
    expect_that(length(res), equals(1))
    expect_that(res[[1]], equals(list(2, 1, 2)))

    # test that unknown operator '?' fails
    expect_that(beer$update("test", list(1L), list(index=0L, ops=list(list(field=1, op="?", arg=0)))), throws_error())

    res <- beer$update("test", list(1L), list(index=0L, ops=list(list(field=2, op="+", arg=0.7))))
    expect_that(length(res), equals(1))
    expect_that(res[[1]], equals(list(1, 2, 1.7)))

    res <- beer$update("test", list(1L), list(index=0L, ops=list(list(field=2, op="-", arg=0.2))))
    expect_that(length(res), equals(1))
    expect_that(res[[1]], equals(list(1, 2, 1.5)))

    # test that operator '^' cannot be used with real numbers
    expect_that(beer$update("test", list(1L), list(index=0L, ops=list(list(field=2, op="^", arg=0.2)))), throws_error())

    system("beectl eval example cleanup.lua")
})

test_that("batch update method works", {
    system("beectl eval example cleanup.lua")
    system("beectl eval example init.lua")
    system("beectl eval example populate_db2.lua")

    beer <- new(Bee)
    expect_that(beer$ping(), is_true())

    res <- beer$update("test", list(1L), list(index=0L, ops=list(list(field=1, op="+", arg=1L),
                                                                list(field=2, op="+", arg=1.1))))
    expect_that(length(res), equals(1))
    expect_that(res[[1]], equals(list(1, 2, 2.1)))

    system("beectl eval example cleanup.lua")
})

test_that("replace operator in update method works", {
    system("beectl eval example cleanup.lua")
    system("beectl eval example init.lua")
    system("beectl eval example populate_db3.lua")

    beer <- new(Bee)
    expect_that(beer$ping(), is_true())

    res <- beer$update("test", list(1L), list(index=0L, ops=list(list(field=1, op="+", arg=1L),
                                                                list(field=2, op="=", arg="aaa"),
                                                                list(field=3, op="=", arg=list("eee", 1)))))
    expect_that(length(res), equals(1))
    expect_that(res[[1]], equals(list(1, 2, "aaa", list("eee", 1), "aa")))

    res <- beer$select("test", 1L, NULL)
    expect_that(length(res), equals(1))
    expect_that(res[[1]], equals(list(1, 2, "aaa", list("eee", 1), "aa")))

    system("beectl eval example cleanup.lua")
})

test_that("detete operator in update method works", {
    system("beectl eval example cleanup.lua")
    system("beectl eval example init.lua")
    system("beectl eval example populate_db3.lua")

    beer <- new(Bee)
    expect_that(beer$ping(), is_true())

    res <- beer$update("test", list(1L), list(index=0L, ops=list(list(field=1, op="+", arg=1L),
                                                                list(field=2, op="#", arg=1),
                                                                list(field=3, op="=", arg="eee"))))
    expect_that(length(res), equals(1))
    expect_that(res[[1]], equals(list(1, 2, "a", "eee")))

    res <- beer$select("test", 1L, NULL)
    expect_that(length(res), equals(1))
    expect_that(res[[1]], equals(list(1, 2, "a", "eee")))

    system("beectl eval example cleanup.lua")
})

test_that("insert operator in update method works", {
    system("beectl eval example cleanup.lua")
    system("beectl eval example init.lua")
    system("beectl eval example populate_db3.lua")

    beer <- new(Bee)
    expect_that(beer$ping(), is_true())

    res <- beer$update("test", list(1L), list(index=0L, ops=list(list(field=1, op="+", arg=1L),
                                                                list(field=2, op="#", arg=1),
                                                                list(field=3, op="!", arg="eee"))))
    expect_that(length(res), equals(1))
    expect_that(res[[1]], equals(list(1, 2, "a", "eee", "aa")))

    res <- beer$select("test", 1L, NULL)
    expect_that(length(res), equals(1))
    expect_that(res[[1]], equals(list(1, 2, "a", "eee", "aa")))

    system("beectl eval example cleanup.lua")
})
