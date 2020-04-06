#ifndef OPTIONS_H
#define OPTIONS_H

#include <memory>

struct drafter_parse_options;

namespace protagonist
{
    struct parse_options_deleter {
        void operator()(drafter_parse_options* obj) noexcept;
    };

    using parse_options_ptr =
        std::unique_ptr<drafter_parse_options, parse_options_deleter>;

    class Options
    {
        parse_options_ptr parse_options_;
        bool serialize_sourcemaps_;

       public:
        Options() noexcept;

        const drafter_parse_options* parseOptions() const noexcept;
        void setNameRequired() noexcept;
        void setSkipGenBodies() noexcept;
        void setSkipGenBodySchemas() noexcept;

        bool serializeSourcemaps() const noexcept;
        void setSerializeSourcemaps() noexcept;

        parse_options_ptr claimParseOptions() noexcept;
    };
}  // namespace protagonist

#endif
