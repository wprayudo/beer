test_that("delete method works", {
    system("beectl eval example cleanup.lua")
    system("beectl eval example init.lua")
    system("beectl eval example populate_db.lua")

    beer <- new(Bee)
    expect_that(beer$ping(), is_true())

    res <- beer$delete("test", 2L, NULL)
    expect_that(length(res), equals(1))
    expect_that(res[[1]], equals(list(2, "bbb")))

    res <- beer$select("test", NULL, NULL)
    expect_that(length(res), equals(5))
    # expect_that(res, equals(list(list(1, "aaa"), list(3, "ccc"), list(4, "ddd"))))

    system("beectl eval example cleanup.lua")
})
