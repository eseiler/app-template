// SPDX-FileCopyrightText: 2006-2024 Knut Reinert & Freie Universität Berlin
// SPDX-FileCopyrightText: 2016-2024 Knut Reinert & MPI für molekulare Genetik
// SPDX-License-Identifier: CC0-1.0

#pragma once

#include <cstdint>
#include <filesystem>

struct config
{
    std::filesystem::path input{};
    std::filesystem::path output{};
    uint8_t kmer{};
    uint8_t threads{};
};
