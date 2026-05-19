#pragma once

#include <nlohmann/json.hpp>

#include <string_view>

namespace hugin {

/**
 * Count every component in a snapshot tree whose `type` matches `type`.
 *
 * Use this when a test wants a structural assertion such as "this screen has
 * two buttons" without first resolving a specific component.
 *
 * @param snapshot The root snapshot returned by `capture_snapshot()`.
 * @param type The Munin component type to match, such as `"button"`.
 * @return The number of matching components in the full snapshot tree.
 */
auto count_components_of_type(
    nlohmann::json const &snapshot, std::string_view type) -> std::size_t;

/**
 * Find the focused leaf component in a snapshot tree.
 *
 * This follows Munin's outward automation view of focus. It returns the most
 * specific focused component that remains visible in the snapshot tree.
 *
 * @param snapshot The root snapshot returned by `capture_snapshot()`.
 * @return A pointer to the focused component JSON object, or `nullptr` if the
 * snapshot tree does not currently contain focus.
 */
auto find_focused_leaf(nlohmann::json const &snapshot)
    -> nlohmann::json const *;

/**
 * Find the first component in a snapshot tree whose `id` matches `id`.
 *
 * Use this when a test wants to target a specific authored control such as
 * `@ok_button` after assigning that `Automation ID` in Munin.
 *
 * @param snapshot The root snapshot returned by `capture_snapshot()`.
 * @param id The authored automation ID to match.
 * @return A pointer to the matching component JSON object, or `nullptr` if no
 * component in the snapshot tree has that ID.
 */
auto find_component_by_id(
    nlohmann::json const &snapshot, std::string_view id)
    -> nlohmann::json const *;

}  // namespace hugin
