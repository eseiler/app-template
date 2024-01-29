#include "cli_test.hpp"

// To prevent issues when running multiple CLI tests in parallel, give each CLI test unique names:
struct fastq_to_fasta : public cli_test
{};

TEST_F(fastq_to_fasta, no_options)
{
    cli_test_result const result = execute_app("app-template");
    std::string_view const expected{"Fastq-to-Fasta-Converter\n"
                                    "========================\n"
                                    "    Try -h or --help for more information.\n"};

    EXPECT_SUCCESS(result);
    EXPECT_EQ(result.out, expected);
    EXPECT_EQ(result.err, "");
}

TEST_F(fastq_to_fasta, fail_no_argument)
{
    cli_test_result const result = execute_app("app-template", "-v");
    std::string_view const expected{"Parsing error. Not enough positional arguments provided (Need at least 1). "
                                    "See -h/--help for more information.\n"};

    EXPECT_FAILURE(result);
    EXPECT_EQ(result.out, "");
    EXPECT_EQ(result.err, expected);
}

TEST_F(fastq_to_fasta, with_argument)
{
    cli_test_result const result = execute_app("app-template", data("in.fastq"));

    EXPECT_SUCCESS(result);
    EXPECT_EQ(result.out, ">seq1\nACGTTTGATTCGCG\n>seq2\nTCGGGGGATTCGCG\n");
    EXPECT_EQ(result.err, "");
}

TEST_F(fastq_to_fasta, with_argument_verbose)
{
    cli_test_result const result = execute_app("app-template", data("in.fastq"), "-v");

    EXPECT_SUCCESS(result);
    EXPECT_EQ(result.out, ">seq1\nACGTTTGATTCGCG\n>seq2\nTCGGGGGATTCGCG\n");
    EXPECT_EQ(result.err, "Conversion was a success. Congrats!\n");
}

TEST_F(fastq_to_fasta, with_out_file)
{
    cli_test_result const result = execute_app("app-template", data("in.fastq"), "-o", "out.fasta");
    std::string const expected = string_from_file(data("out.fasta"));
    std::string const actual = string_from_file("out.fasta");

    EXPECT_SUCCESS(result);
    EXPECT_EQ(result.out, "");
    EXPECT_EQ(result.err, "");
    EXPECT_EQ(actual, expected);
}
