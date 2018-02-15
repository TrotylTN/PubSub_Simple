# export GOOGLE_APPLICATION_CREDENTIALS="/Users/Tiannan/Downloads/MyProject49385-ed1ed8d30fa8.json"
from google.cloud import pubsub_v1

def Join():
    # Instantiates clients
    publisher = pubsub_v1.PublisherClient()
    subscriber = pubsub_v1.SubscriberClient()
    # 0 is publisher, 1 is subscriber
    return [publisher, subscriber]

def CreateTopic(clients, a_topic_name):
    publisher = clients[0]
    # Create the topic.
    topic_path = publisher.topic_path("complete-button-195306", a_topic_name)
    try:
        publisher.create_topic(topic_path)
    except Exception:
        pass
    return topic_path

def Subscribe(clients, topic_path):
    subscriber = clients[1]
    try:
        subscription_path = subscriber.subscription_path("complete-button-195306", topic_path)
        subscription = subscriber.create_subscription(subscription_path, topic_path)
    except:
        raise Exception("Fail: topic is non-existent")
    print("Subscribed " + topic_path)

def Unsubscribe(clients, topic_path):
    subscriber = clients[1]
    try:
        subscription_path = subscriber.subscription_path("complete-button-195306", topic_path)
        subscriber.delete_subscription(subscription_path)
    except:
        raise Exception("Fail: topic is non-existent")
    print("Unsubscribed " + topic_path)


def Publish(clients, topic_path, article):
    publisher = clients[0]
    article = article.encode('utf-8')
    publisher.publish(topic_path, data=data)
