// SPDX-FileCopyrightText: 2006-2024 Knut Reinert & Freie Universität Berlin
// SPDX-FileCopyrightText: 2016-2024 Knut Reinert & MPI für molekulare Genetik
// SPDX-License-Identifier: CC0-1.0

#include <cstdint>
#include <filesystem>

#include <sharg/all.hpp>

struct config
{
    std::filesystem::path index{};
    std::filesystem::path output{};
    double threshold{0.7};
    uint8_t threads{1};
};

void search(sharg::parser & parser)
{
    config config{};
    parser.add_option(config.index,
                      sharg::config{.short_id = 'i',
                                    .long_id = "input",
                                    .description = "Input",
                                    .required = true,
                                    .validator = sharg::input_file_validator{}});

    parser.add_option(config.output,
                      sharg::config{.short_id = 'o',
                                    .long_id = "output",
                                    .description = "Output",
                                    .required = true,
                                    .validator = sharg::output_file_validator{}});

    parser.add_option(config.threshold,
                      sharg::config{.short_id = '\0',
                                    .long_id = "threshold",
                                    .description = "Threshold.",
                                    .validator = sharg::arithmetic_range_validator{0.0, 1.0}});

    parser.add_option(config.threads,
                      sharg::config{.short_id = 't',
                                    .long_id = "threads",
                                    .description = "Threads.",
                                    .validator = sharg::arithmetic_range_validator{1, 255}});

    try
    {
        parser.parse();
    }
    catch (sharg::parser_error const & ext)
    {
        std::cerr << "Parsing error. " << ext.what() << '\n';
        std::exit(EXIT_FAILURE);
    }
}
