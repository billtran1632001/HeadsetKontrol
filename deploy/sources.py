class SourcesMeta(type):

    def __getitem__(cls, key):
        return cls._config[key]

    def __setitem__(cls, key, value):
        if key == "timeout" and value <= 0:
            raise ValueError("Timeout must be positive")
        cls._config[key] = value

    def __contains__(cls, key):
        return key in cls._config


class Sources(metaclass=SourcesMeta):
    _config = {}
