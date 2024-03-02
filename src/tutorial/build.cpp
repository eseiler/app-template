// SPDX-FileCopyrightText: 2006-2024 Knut Reinert & Freie Universität Berlin
// SPDX-FileCopyrightText: 2016-2024 Knut Reinert & MPI für molekulare Genetik
// SPDX-License-Identifier: CC0-1.0

#include <cstdint>
#include <filesystem>

#include <sharg/all.hpp>

#include <seqan3/io/sequence_file/input.hpp>
#include <seqan3/search/views/kmer_hash.hpp>

#include <hibf/cereal/path.hpp>
#include <hibf/config.hpp>
#include <hibf/hierarchical_interleaved_bloom_filter.hpp>

struct dna4_traits : seqan3::sequence_file_input_default_traits_dna
{
    using sequence_alphabet = seqan3::dna4;
};

using sequence_file_input = seqan3::sequence_file_input<dna4_traits, seqan3::fields<seqan3::field::seq>>;

struct myindex
{
    uint8_t kmer{};
    std::vector<std::filesystem::path> input_files{};
    seqan::hibf::hierarchical_interleaved_bloom_filter hibf{};

    myindex() = default;
    myindex & operator=(myindex const &) = default;
    myindex(myindex const &) = default;
    myindex(myindex &&) = default;
    myindex & operator=(myindex &&) = default;

    explicit myindex(uint8_t const kmer,
                     std::vector<std::filesystem::path> input_files,
                     seqan::hibf::hierarchical_interleaved_bloom_filter hibf) :
        kmer{kmer},
        input_files{std::move(input_files)},
        hibf{std::move(hibf)}
    {}

    template <typename archive_t>
    void CEREAL_SERIALIZE_FUNCTION_NAME(archive_t & archive)
    {
        archive(kmer);
        archive(input_files);
        archive(hibf);
    }
};

struct config
{
    std::filesystem::path input{};
    std::vector<std::filesystem::path> input_files{};
    std::filesystem::path output{};
    uint8_t kmer{};
    uint8_t threads{1};
};

void build_hibf(config & config)
{
    auto input_lambda = [&config](size_t const user_bin_index, seqan::hibf::insert_iterator it)
    {
        sequence_file_input fin{config.input_files[user_bin_index]};
        for (auto && [seq] : fin)
            for (auto && hash : seq | seqan3::views::kmer_hash(seqan3::ungapped{config.kmer}))
                it = hash;
    };

    seqan::hibf::config hibf_config{.input_fn = input_lambda,
                                    .number_of_user_bins = config.input_files.size(),
                                    .number_of_hash_functions = 2u,
                                    .maximum_fpr = 0.05,
                                    .threads = config.threads};

    seqan::hibf::hierarchical_interleaved_bloom_filter hibf{hibf_config};
    myindex index{config.kmer, std::move(config.input_files), std::move(hibf)};

    std::ofstream fout{config.output};
    cereal::BinaryOutputArchive oarchive{fout};
    oarchive(index);
}

void read_input_files(config & config)
{
    std::ifstream file{config.input};
    std::string line{};

    while (std::getline(file, line))
        config.input_files.emplace_back(line);

    if (config.input_files.empty())
        throw sharg::validation_error{"No input files found in " + config.input.string() + "."};

    std::ranges::for_each(config.input_files, sharg::input_file_validator{{"fa", "fasta"}});
}

void build(sharg::parser & parser)
{
    config config{};
    parser.add_option(config.input,
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

    parser.add_option(config.kmer,
                      sharg::config{.short_id = 'k',
                                    .long_id = "kmer",
                                    .description = "Kmer",
                                    .required = true,
                                    .validator = sharg::arithmetic_range_validator{1, 32}});

    parser.add_option(config.threads,
                      sharg::config{.short_id = 't',
                                    .long_id = "threads",
                                    .description = "Threads.",
                                    .validator = sharg::arithmetic_range_validator{1, 255}});

    try
    {
        parser.parse();
        read_input_files(config);
    }
    catch (sharg::parser_error const & ext)
    {
        std::cerr << "Parsing error. " << ext.what() << '\n';
        std::exit(EXIT_FAILURE);
    }

    build_hibf(config);
}
