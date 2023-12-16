# Link Shortener

Gonna be a high-perfomance server using lots of tools and techniques. In development nowadays!

# API
Supported requests:
1) "/url" GET with body example: {"short": "lJDcTuT5"} -> gets original from the short one
2) "/url" POST with body example: {"original": "www.google.com"} -> creates short version and returns it as reply

# Deps
1) https://github.com/sogou/workflow
2) https://github.com/idealvin/coost
