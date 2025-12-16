# Env variables for scripts
- EVENT_NUM - id of an event. `2024` is the event from the year 2024, `1` is the first story.
- QUEST_NUM
- SEED - seed assigned to the user. You can get it by looking at /me endpoint.
- API_TOKEN - uuid4 from a cookie.

> When it's not logical for a script to use some env variable (like `API_TOKEN` for `get_title.sh`),
> you don't have to provide it.


