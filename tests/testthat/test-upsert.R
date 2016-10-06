test_that("upsert method works", {
    system("beectl eval example cleanup.lua")
    system("beectl eval example init.lua")
    system("beectl eval example populate_db2.lua")

    beer <- new(Bee)
    expect_that(beer$ping(), is_true())

    beer$upsert("test", list(1L, 100L, 200L), list(index=0L, ops=list(list(field=1, op="+", arg=1))))
    res <- beer$select("test", 1L, NULL)
    expect_that(length(res), equals(1))
    expect_that(res[[1]], equals(list(1, 2, 1)))

    beer$upsert("test", list(3L, 100L, 200L), list(index=0L, ops=list(list(field=1, op="+", arg=1))))
    res <- beer$select("test", 3L, NULL)
    expect_that(length(res), equals(1))
    expect_that(res[[1]], equals(list(3, 100, 200)))

    system("beectl eval example cleanup.lua")
})
