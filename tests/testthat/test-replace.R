test_that("replace method works", {
    system("beectl eval example cleanup.lua")
    system("beectl eval example init.lua")
    system("beectl eval example populate_db.lua")

    beer <- new(Bee)
    expect_that(beer$ping(), is_true())

    res <- beer$replace("test", list(1L, "eee", T, F, 3.14))
    expect_that(length(res), equals(1))
    expect_that(res[[1]], equals(list(1, "eee", TRUE, FALSE, 3.14)))

    res <- beer$select("test", 1L, NULL)
    expect_that(length(res), equals(1))
    expect_that(res, equals(list(list(1, "eee", TRUE, FALSE, 3.14))))

    system("beectl eval example cleanup.lua")
})
