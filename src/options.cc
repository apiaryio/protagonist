#include "options.h"

#include <cassert>

#include "drafter.h"

using namespace protagonist;

namespace
{
    void ensure_parse_options(parse_options_ptr& opts) noexcept
    {
        if (!opts) {
            opts.reset(drafter_init_parse_options());
        }
    }
}  // namespace

void parse_options_deleter::operator()(drafter_parse_options* obj) noexcept
{
    drafter_free_parse_options(obj);
}

Options::Options() noexcept
    : parse_options_{nullptr}, serialize_sourcemaps_{false}
{
}

const drafter_parse_options* Options::parseOptions() const noexcept
{
    return parse_options_.get();
}

parse_options_ptr Options::claimParseOptions() noexcept
{
    return std::move(parse_options_);
}

void Options::setSerializeSourcemaps() noexcept
{
    serialize_sourcemaps_ = true;
}

bool Options::serializeSourcemaps() const noexcept
{
    return serialize_sourcemaps_;
}

void Options::setNameRequired() noexcept
{
    ensure_parse_options(parse_options_);
    drafter_set_name_required(parse_options_.get());
}

void Options::setSkipGenBodies() noexcept
{
    ensure_parse_options(parse_options_);
    drafter_set_skip_gen_bodies(parse_options_.get());
}

void Options::setSkipGenBodySchemas() noexcept
{
    ensure_parse_options(parse_options_);
    drafter_set_skip_gen_body_schemas(parse_options_.get());
}
